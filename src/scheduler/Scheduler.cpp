#include "Scheduler.h"
#include "misc/Bazel.h"
#include "misc/Js.h"
#include "misc/Log.h"
#include "misc/kafkawrap/KafkaReaderMock.h"
#include "misc/kafkawrap/KafkaWriterMock.h"
#include <functional>

namespace contentv1 {

SchedulerConfig::SchedulerConfig() {
  addArrays(s_cfg_arrays);
  addStrings(s_cfg_strings);
  addInts(s_cfg_ints);
}

Scheduler::Scheduler(const SchedulerConfig &_config, bool _dry_run,
                     bool _in_bazel)
    : m_dry_run(_dry_run) {

  _config.validate("Scheduler::Scheduler");

  LOG("Init host db");
  m_host_db.reset(new HostDb{_config});
  LOG("Init url db");
  m_url_db.reset(new UrlDb{_config});
  LOG("Init urls days db");
  m_urls_days_db.reset(new UrlsDaysDb{_config});
  LOG("Init crawled db");
  m_crawled_db.reset(new CrawledDb{_config});
  LOG("Init scheduler's db's done");

  if (_dry_run) {
    std::string dataset_path = "." + s_gened_visits_dataset_rel_path;
    if (_in_bazel)
      dataset_path =
          misc::get_bazel_test_data_path(s_gened_visits_dataset_rel_path);

    LOG("Loading dataset for dry run: " << dataset_path);

    auto gened_visits_dataset =
        misc::KafkaReaderMock::loadNewlineFile(dataset_path);
    LOG("DONE, visits: " << gened_visits_dataset.size());
    LOG("Init KafkaReaderMock");
    m_kafka_reader.reset(new misc::KafkaReaderMock{gened_visits_dataset});
    LOG("Init KafkaWriterMock");
    m_kafka_writer.reset(new misc::KafkaWriterMock{
        _config[EnqueueLoopConfig::STR_KAFKA_PUSH_TOPIC].asString()}); // sim
    LOG("Init Kafka mocks done");
  } else {
    m_kafka_reader.reset(new misc::KafkaReader{
        _config[SchedulerConfig::STR_KAFKA_INPUT_STREAM_VISITS_TOPIC]
            .asString(),
        misc::get_str_array_from_json(
            _config[SchedulerConfig::STR_ARRAY_KAFKA_BROKERS])});
    m_kafka_writer.reset(new misc::KafkaWriter{misc::get_str_array_from_json(
        _config[SchedulerConfig::STR_ARRAY_KAFKA_BROKERS])});
  }

  LOG("Init UrlFreqStatsLF");
  // fill from UrlsDaysDb
  m_url_freq_stats.reset(new UrlFreqStatsLF{
      _config[SchedulerConfig::INT_URL_FREQ_STATS_LF_MERGE_INTERVAL_SECS]
          .asInt()});

  LOG("Init LoadLoop");
  m_load_loop.reset(
      new LoadLoop{_config, m_url_freq_stats, m_urls_days_db, m_kafka_reader});
  LOG("Init EnqueueLoop");
  m_enqueue_loop.reset(new EnqueueLoop{_config, m_url_freq_stats, m_host_db,
                                       m_url_db, m_crawled_db, m_kafka_writer});

  if (_dry_run) {
    LOG("Calling m_load_loop->_loopImpl()");
    auto cnt = m_load_loop->_loopImpl();
    LOG("DONE, loaded: " << cnt);
    // sleep(2); // if empty
    LOG("Calling m_urls_days_db->waitMerged()..");
    m_urls_days_db->waitMerged();
    LOG("DONE");
  }

  LOG("Calling Scheduler::_fillUrlFreqStats() from m_urls_days_db..");
  m_loaded_uts_count =
      Scheduler::_fillUrlFreqStats(m_urls_days_db, m_url_freq_stats);
  LOG("DONE");
  LOG("Calling m_url_freq_stats->waitMerged()..");
  m_url_freq_stats->waitMerged();
  LOG("DONE");

  if (_dry_run) {
    // calling enqueue loop
    LOG("Calling m_enqueue_loop->_loopImpl()..");
    auto pushed_cnt = m_enqueue_loop->_loopImpl();
    LOG("DONE, pushed: " << pushed_cnt);
  }

  LOG("DONE. Exiting Scheduler::Scheduler");
}

Cnt Scheduler::_fillUrlFreqStats(UrlsDaysDb::Ptr _urls_days_db,
                                 UrlFreqStatsLF::Ptr _url_freq_stats) {
  Cnt all_uts = 0;
  auto now = misc::get_now();
  _urls_days_db->iterateAll(
      [&](const Url &_url, const std::vector<UrlTpStat> &_utsv) {
        for (size_t i = 0; i < _utsv.size(); ++i) {
          _url_freq_stats->insertVisitsPack(_utsv[i], now);
          ++all_uts;
        }
      });
  return all_uts;
}

Scheduler::~Scheduler() {
  LOG("~Scheduler() start");
  stop();
  if (!m_dry_run)
    join();
  LOG("~Scheduler() finish");
}

FetchStats Scheduler::getStats() const {
  return FetchStats{0, m_success_count.load(), m_fail_count.load(),
                    m_robots_count.load(), m_robots_fail_count.load()};
}

void Scheduler::start() {
  m_load_th.reset(
      new std::thread{std::bind(&LoadLoop::loop, m_load_loop.get())});
  m_enqueue_th.reset(
      new std::thread{std::bind(&EnqueueLoop::loop, m_enqueue_loop.get())});
}

void Scheduler::stop() {
  if (m_load_loop)
    m_load_loop->stop();
  if (m_enqueue_loop)
    m_enqueue_loop->stop();
}

void Scheduler::join() {
  if (m_load_th)
    m_load_th->join();
  if (m_enqueue_th)
    m_enqueue_th->join();
}

///// API IMPLEMENTATION
//
void Scheduler::tryFetchUrls(const std::vector<GFQRecord> &_try_urls,
                             std::vector<GFQRecord> &_allowed_urls) {
  std::vector<Url> need_robots; // not used, for debug only
  m_host_db->tryCrawl(_try_urls, misc::get_now(), _allowed_urls, need_robots);
  LOG("Scheduler::tryFetchUrls try_urls: " << _try_urls.size());
  LOG("Scheduler::tryFetchUrls allowed: " << _allowed_urls.size());
  LOG("Scheduler::tryFetchUrls need_robots: " << need_robots.size());
}

void Scheduler::onFetched(const std::vector<FetchResult> &_results) {

  m_url_db->onFetched(_results);
  m_host_db->onFetched(_results);
  m_crawled_db->onFetched(_results);

  _incStats(_results);

  // TODO call CH here
}

void Scheduler::_incStats(const std::vector<FetchResult> &_results) {
  for (size_t i = 0; i < _results.size(); ++i) {
    if (_results[i].isOk()) {
      if (_results[i].isRobots())
        ++m_robots_count;
      else
        ++m_success_count;
    } else { // fail
      if (_results[i].isRobots())
        ++m_robots_fail_count;
      else
        ++m_fail_count;
    }
  }
}

} // namespace contentv1
