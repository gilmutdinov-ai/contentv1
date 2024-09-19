#include "scheduler/default/ut/EnqueueLoopTester.h"
#include "misc/Time.h"
#include "misc/kafkawrap/KafkaWriterMock.h"
#include "tests/common/robots_str.h"
#include "tests/common/test_urls.h"
#include "tests/common/test_visits.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <iostream>

namespace contentv1 {

EnqueueLoopTester::EnqueueLoopTester() {
  _setUp();
  _simVisitsAsLoadLoop();
  _testAfterState();
  _testCheckEnqueue();
}

void EnqueueLoopTester::_setUp() {

  std::filesystem::remove_all("/tmp/enq_loop_ut");
  /////////// FREQ STATS INIT
  m_url_freq_stats.reset(new UrlFreqStats{});
  /////////// HOST DB INIT
  m_host_db = _initHostDb();
  REQUIRE(m_host_db->sizeRobots() == 0);
  /////////// URL DB INIT
  m_url_db = _initUrlDb();
  REQUIRE(m_url_db->size() == 0);
  /////////// CRAWLED DB INIT
  m_crawled_db = _initCrawledDb();
  /////////// KAFKA WRITER MOCK
  m_kafka_writer_mock.reset(new misc::KafkaWriterMock());

  /////////// ENQUEUE LOOP INIT - OUR PATIENT
  m_enq_loop = _initEnqLoop(m_url_freq_stats, m_host_db, m_url_db, m_crawled_db,
                            m_kafka_writer_mock);
}

EnqueueLoop::Ptr EnqueueLoopTester::_initEnqLoop(
    UrlFreqStats::Ptr _url_freq_stats, HostDb::Ptr _host_db, UrlDb::Ptr _url_db,
    CrawledDb::Ptr _crawled_db, misc::KafkaWriterI::Ptr _kafka_writer_mock) {

  EnqueueLoopConfig cfg;
  // std::cout << cfg.help() << std::endl;
  cfg[EnqueueLoopConfig::INT_ENQ_LOOP_INTERVAL] = 1;
  cfg[EnqueueLoopConfig::STR_ENQ_LOOP_DB_PATH] = m_enq_loop_db_path;
  cfg[CrawlTargetsConfig::INT_TARGET_DAY_CRAWL] = m_target_day_crawl;
  cfg[EnqueueLoopConfig::STR_KAFKA_PUSH_TOPIC] = m_push_topic;

  cfg.onParsed();
  cfg.validate("EnqueueLoopTester::_initEnqLoop");

  return EnqueueLoop::Ptr{new EnqueueLoop{cfg, _url_freq_stats, _host_db,
                                          _url_db, _crawled_db,
                                          _kafka_writer_mock}};
}

HostDb::Ptr EnqueueLoopTester::_initHostDb() {

  HostDbConfig cfg;
  cfg[HostDbConfig::STR_HOST_DB_PATH] = m_host_db_path;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_DAY_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_MINUTE_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_HOUR_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_PARALLEL_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_SYSTEM_PARALLEL_CRAWL] = 100;

  cfg.onParsed();
  cfg.validate("EnqueueLoopTester::_initHostDb");

  std::filesystem::remove_all(m_host_db_path);
  return HostDb::Ptr{new HostDb{cfg}};
}

UrlDb::Ptr EnqueueLoopTester::_initUrlDb() {

  UrlDbConfig cfg;
  cfg[UrlDbConfig::STR_URL_DB_PATH] = m_url_db_path;
  cfg[UrlDbConfig::INT_URL_DB_SNAPSHOT_INTERVAL_SEC] =
      m_url_db_snapshot_interval_sec;

  cfg.onParsed();
  cfg.validate("EnqueueLoopTester::_initUrlDb");

  std::filesystem::remove_all(m_url_db_path);
  return UrlDb::Ptr{new UrlDb{cfg}};
}

CrawledDb::Ptr EnqueueLoopTester::_initCrawledDb() {

  // events from previous tests saved :-)
  std::filesystem::remove_all(m_crawled_db_path);
  CrawledDbConfig cfg;
  cfg[CrawledDbConfig::STR_CRAWLED_DB_PATH] = std::string(m_crawled_db_path);
  cfg[CrawledDbConfig::INT_CRAWLED_DB_MERGE_INTERVAL_SECS] = 2;
  cfg[CrawledDbConfig::INT_CRAWLED_DB_SNAPSHOT_INTERVAL_SECS] = 3;

  cfg.onParsed();
  cfg.validate("EnqueueLoopTester::_initCrawledDb");

  return CrawledDb::Ptr(new CrawledDb{cfg});
}

void EnqueueLoopTester::_simVisitsAsLoadLoop() {

  Url url = {"https://www.kp.ru/"}; // already normalized
  Url noindex_url{"https://www.kp.ru/banners/x/"};
  auto now = misc::get_now();
  test_visits(url, now,
              [&](const UrlVisit &_uv) { m_url_freq_stats->insert(_uv, now); });
  test_visits(noindex_url, now,
              [&](const UrlVisit &_uv) { m_url_freq_stats->insert(_uv, now); });

  Url expect_robots_url{"https://www.kp.ru/robots.txt"};
  // _filter before robots
  {
    Cnt top{10};
    std::vector<UrlFreq> push_urls;
    auto decisions = m_enq_loop->_filter(top, push_urls);
    REQUIRE(push_urls.size() == 1);
    REQUIRE(push_urls[0].first == expect_robots_url);
  }
  // add robots
  m_host_db->onRobotsTxt(expect_robots_url, g_robots_test_str);
  // _filter after robots
  {
    Cnt top{10};
    std::vector<UrlFreq> push_urls;
    auto decisions = m_enq_loop->_filter(top, push_urls);
    REQUIRE(push_urls.size() == 1);
    REQUIRE(push_urls[0].first == url);
  }
  // gened lots of urls with known sorting
  {
    for (size_t i = 1; i <= m_urls_count; ++i) {

      test_visits_x(i, gen_test_url(i), now, [&](const UrlVisit &_uv) {
        m_url_freq_stats->insert(_uv, now);
      });
    }
    // test sorting
    Cnt top{10};
    std::vector<UrlFreq> push_urls;
    auto decisions = m_enq_loop->_filter(top, push_urls);
    /*
      top != push_count (в реальности)
        потому что
         1. роботс (их нет или запрещено)
         2. политика выкачивания
      поэтому и нужен бинпоиск (на самом деле просто x2) чтобы подбирая top
      попасть push_count -> target_crawl
    */

    // проверяем два самых популярных урла, у которых есть роботс и они
    // разрешены политикой выкачивания (база чистая)
    auto top_url = push_urls[0].first;
    REQUIRE(parse_test_url_id(top_url) == static_cast<char>(m_urls_count));
    auto top2_url = push_urls[1].first;
    REQUIRE(parse_test_url_id(top2_url) == static_cast<char>(m_urls_count - 1));
  }
}

void EnqueueLoopTester::_printPushUrls(const std::vector<UrlFreq> &_ufv,
                                       size_t _limit) {
  std::cout << "EnqueueLoopTester::_printPushUrls push_count: " << _ufv.size()
            << std::endl;
  for (size_t i = 0; i < _ufv.size(); ++i) {
    if (_limit != 0 && i == _limit) {
      std::cout << ".." << std::endl;
      break;
    }
    auto url = _ufv[i].first;
    auto freq = _ufv[i].second;
    auto url_id = parse_test_url_id(url);

    std::cout << url_id << ": " << url << std::endl;
  }
}

void EnqueueLoopTester::_testAfterState() {
  // change state in urldb for some test urls and then check

  auto now = misc::get_now();
  // top half set enqueued
  {
    std::vector<Url> to_enqueue_urls;
    for (size_t i = m_urls_count; i >= m_urls_count / 2 + 1; --i)
      to_enqueue_urls.push_back(gen_test_url(i));

    m_url_db->setEnqueued(to_enqueue_urls, now);

  } // check after first half set enqueued
  {
    Cnt top{10};
    std::vector<UrlFreq> push_urls;
    auto decisions = m_enq_loop->_filter(top, push_urls);
    REQUIRE(push_urls.empty());
  }
  {
    Cnt top{m_urls_count / 2 + 20};
    std::vector<UrlFreq> push_urls;
    auto decisions = m_enq_loop->_filter(top, push_urls);
    REQUIRE(push_urls.size() > 10);
  }
  // searchTop
  {
    Cnt push_target{20};
    std::vector<UrlFreq> push_urls;
    auto decisions = m_enq_loop->searchTop(push_target, push_urls);
    REQUIRE(push_urls.size() == 20);
    REQUIRE(decisions.size() > push_urls.size());
    REQUIRE(push_urls[0].first == gen_test_url(m_urls_count / 2));

    //_printPushUrls(push_urls);
  }
  // Если их меньше, чем запрошено
  {
    Cnt push_target{m_urls_count / 2 + 5};
    std::vector<UrlFreq> push_urls;
    auto decisions = m_enq_loop->searchTop(push_target, push_urls);

    //_printPushUrls(push_urls, 5);
    REQUIRE(push_urls.size() >= m_urls_count / 2);
    REQUIRE(decisions.size() >= m_urls_count);
  }
}

void EnqueueLoopTester::_testCheckEnqueue() {

  std::vector<UrlFreq> push_urls;
  m_enq_loop->_checkEnqueueImpl(push_urls);
  std::cout << "EnqueueLoopTester::_testCheckEnqueue: " << push_urls.size()
            << std::endl;
  REQUIRE(push_urls.size() == 51);
}

} // namespace contentv1
