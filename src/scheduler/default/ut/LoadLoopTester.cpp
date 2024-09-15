#include "scheduler/default/ut/LoadLoopTester.h"
#include "misc/Bazel.h"
#include "misc/kafkawrap/KafkaReaderMock.h"
#include "scheduler/default/Scheduler.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

namespace contentv1 {

LoadLoopTester::LoadLoopTester() {

  std::filesystem::remove_all("/tmp/load_loop_ut");

  // config from Scheduler
  auto config_path{misc::get_bazel_test_data_path(s_conf_rel_path)};
  std::cout << "Loading config: " << config_path << std::endl;
  {
    std::ifstream t(config_path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::cout << buffer.str();
  }

  SchedulerConfig _config;
  _config.parseFile(config_path);
  // std::cout << cfg.help() << std::endl;
  _config.validate();
  // m_url_freq_stats
  m_url_freq_stats.reset(new UrlFreqStatsLF{
      _config[SchedulerConfig::INT_URL_FREQ_STATS_LF_MERGE_INTERVAL_SECS]
          .as<int>()});
  // m_urls_days_db
  std::filesystem::remove_all(
      _config[UrlsDaysDbConfig::STR_URLS_DAYS_DB_PATH].as<std::string>());
  m_urls_days_db.reset(new UrlsDaysDb{_config});
  // m_kafka_reader
  {
    std::string dataset_path{
        misc::get_bazel_test_data_path(s_gened_visits_dataset_rel_path)};

    auto gened_visits_dataset =
        misc::KafkaReaderMock::loadNewlineFile(dataset_path);
    REQUIRE(gened_visits_dataset.size() != 0);
    std::cout << "LoadLoopTester loaded gened visits: "
              << gened_visits_dataset.size() << std::endl;
    m_kafka_reader.reset(
        new misc::KafkaReaderMock{std::move(gened_visits_dataset)});
  }
  m_load_loop.reset(
      new LoadLoop{_config, m_url_freq_stats, m_urls_days_db, m_kafka_reader});

  // pre condition
  auto url_freq_stats_max_pre = m_url_freq_stats->max();
  auto urls_days_size_pre = m_urls_days_db->size();

  // OUR PATIENT: loading test data
  auto loaded_cnt = m_load_loop->_loopImpl();
  std::cout << "_loopImpl loaded_cnt: " << loaded_cnt << std::endl;

  // post condition
  m_url_freq_stats->waitMerged();
  auto url_freq_stats_max_post = m_url_freq_stats->max();
  m_urls_days_db->waitMerged();
  auto urls_days_size_post = m_urls_days_db->size();

  std::cout << "url_freq_stats_max_post " << url_freq_stats_max_post
            << std::endl;
  REQUIRE(url_freq_stats_max_post > url_freq_stats_max_pre);
  std::cout << "urls_days_size_post " << urls_days_size_post << std::endl;
  REQUIRE(urls_days_size_post > urls_days_size_pre);
}
} // namespace contentv1
