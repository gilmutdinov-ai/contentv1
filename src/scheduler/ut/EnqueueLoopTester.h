#pragma once

#include "scheduler/EnqueueLoop.h"

/*

EnqLoop - самый верхнеуровневый актор в шедулере, поэтому тест развесистый

Всё остальное вроде как должно быть проще

*/

namespace contentv1 {
class EnqueueLoopTester {
private:
  // CONFIG

  // HOST DB
  static constexpr const char *m_host_db_path{"/tmp/enq_loop_ut/host_db_test"};
  // URL DB
  static constexpr const char *m_url_db_path = {"/tmp/enq_loop_ut/url_db_test"};
  static constexpr int m_url_db_snapshot_interval_sec{2};
  // CRAWLED DB
  static constexpr const char *m_crawled_db_path{
      "/tmp/enq_loop_ut/test_crawled_db"};

  // ENQ LOOP
  static constexpr const char *m_enq_loop_db_path{
      "/tmp/enq_loop_ut/enq_loop_db"};
  static constexpr Cnt m_target_day_crawl = 100000;

  inline static const std::string m_push_topic{"global_fetch_queue"};

  /////////// URL FREQ STATS
  UrlFreqStats::Ptr m_url_freq_stats;
  /////////// HOST DB
  HostDb::Ptr m_host_db;
  /////////// URL DB
  UrlDb::Ptr m_url_db;
  /////////// CRAWLED DB
  CrawledDb::Ptr m_crawled_db;
  /////////// KAFKA WRITER MOCK
  misc::KafkaWriterI::Ptr m_kafka_writer_mock;
  /////////// ENQ LOOP - OUR PATIENT
  EnqueueLoop::Ptr m_enq_loop;

  /////////// TEST CONFIG
  static constexpr Cnt m_urls_count = 100; // better even

public:
  EnqueueLoopTester();

private:
  void _setUp();
  void _simVisitsAsLoadLoop();
  void _testAfterState();
  void _testCheckEnqueue();

  void _printPushUrls(const std::vector<UrlFreq> &_ufv, size_t _limit = 0);

  static HostDb::Ptr _initHostDb();
  static UrlDb::Ptr _initUrlDb();
  static CrawledDb::Ptr _initCrawledDb();

  static EnqueueLoop::Ptr
  _initEnqLoop(UrlFreqStats::Ptr _url_freq_stats, HostDb::Ptr _host_db,
               UrlDb::Ptr _url_db, CrawledDb::Ptr _crawled_db,
               misc::KafkaWriterI::Ptr _kafka_writer_mock);
};
} // namespace contentv1
