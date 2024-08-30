#include "worker/ut/FetchLoopTester.h"
#include "core/GFQRecord.h"
#include "core/UrlVisit.h"
#include "misc/Bazel.h"
#include "misc/Time.h"
#include "misc/kafkawrap/KafkaReaderMock.h"
#include "scheduler/SchedulerStub.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

namespace contentv1 {

static std::vector<std::string>
loadNewlineFileUniqueUrlVisitsUrls(const std::string &_path, size_t _skip) {
  std::vector<std::string> ret;
  std::set<Url> pushed_urls;
  auto lines = misc::KafkaReaderMock::loadNewlineFile(_path);
  for (size_t i = 0; i < lines.size(); ++i) {
    UrlVisit uv{lines[i]};
    if (pushed_urls.find(uv.url) == pushed_urls.end()) {
      if (pushed_urls.size() > _skip)
        ret.push_back(uv.url);
      pushed_urls.insert(uv.url);
    }
  }
  return ret;
}

std::vector<std::string> FetchLoopTester::genGFQDataset(bool _under_bazel_test,
                                                        Ts _attempt_ts) {

  std::string dataset_path{"." + s_gened_visits_dataset_rel_path};
  if (_under_bazel_test)
    dataset_path =
        misc::get_bazel_test_data_path(s_gened_visits_dataset_rel_path);

  size_t skip = 42;
  auto gened_visits_unique_urls =
      loadNewlineFileUniqueUrlVisitsUrls(dataset_path, skip);

  REQUIRE(gened_visits_unique_urls.size() != 0);

  std::cout << "genGFQDataset loaded gened unique urls: "
            << gened_visits_unique_urls.size() << std::endl;

  std::vector<std::string> ret;
  // have to be at least 1 robots in test
  ret.push_back(
      GFQRecord{"https://www.nbcnews.com/robots.txt", _attempt_ts}.dump());
  for (size_t i = 0; i < gened_visits_unique_urls.size(); ++i) {
    GFQRecord gfq;
    gfq.url = gened_visits_unique_urls[i];
    gfq.attempt_ts = _attempt_ts;
    ret.push_back(gfq.dump());
  }

  return ret;
}

FetchLoopTester::FetchLoopTester(bool _under_bazel_test) {

  auto attempt_ts = misc::to_int64(misc::get_now());
  // mimic global_fetch_queue
  m_kafka_reader.reset(
      new misc::KafkaReaderMock{genGFQDataset(_under_bazel_test, attempt_ts)});

  // scheduler stub
  SchedulerStub *sched_stub = new SchedulerStub{};
  m_scheduler_api.reset(sched_stub);

  std::filesystem::remove_all("/tmp/worker_ut/");
  // PAGEDB
  {
    PageDbConfig pagedb_config;
    pagedb_config[PageDbConfig::STR_PAGE_DB_PATH] = "/tmp/worker_ut/page_db";
    m_page_db.reset(new PageDb{pagedb_config});
  }
  // OUR PATIENT
  FetchLoopConfig fetch_config{};
  fetch_config.max_requests_in_batch = 4;
  fetch_config.kafka_read_batch_size = 4;

  m_fetch_loop.reset(
      new FetchLoop(m_kafka_reader, m_scheduler_api, m_page_db, fetch_config));

  m_fetch_loop->_loopImpl();
  m_fetch_loop->waitMerged();
  sleep(3);

  std::cout << "Finished test, desctructing..\n";

  // Что-то скачали
  REQUIRE(m_fetch_loop->m_success_count != 0);

  // Отстучали в шедулер
  REQUIRE(sched_stub->m_crawled.size() != 0);
  for (size_t i = 0; i < sched_stub->m_crawled.size(); ++i)
    REQUIRE(sched_stub->m_crawled[i].attempt_ts == attempt_ts);
  // Остучали в шедулер про роботс
  REQUIRE(sched_stub->m_robots.size() != 0);
  for (size_t i = 0; i < sched_stub->m_robots.size(); ++i) {
    REQUIRE(sched_stub->m_robots[i].first.ends_with("/robots.txt"));
    REQUIRE(!sched_stub->m_robots[i].second.empty());
  }
  // Нет ошибок роботс
  REQUIRE(sched_stub->m_robots_failed.empty());
  // Нет ошибок страниц
  auto *descriptor = CrawlAttemptsPb_AttemptStatus_descriptor();
  for (size_t i = 0; i < sched_stub->m_failed.size(); ++i) {
    std::cout << sched_stub->m_failed[i].first.url << " ";
    std::cout
        << descriptor->FindValueByNumber(sched_stub->m_failed[i].second)->name()
        << std::endl;
  }
  REQUIRE(sched_stub->m_failed.empty());
}
} // namespace contentv1
