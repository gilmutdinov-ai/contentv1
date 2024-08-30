#include <catch2/catch_test_macros.hpp>

#include "ldb/HostDb.h"
#include "misc/Time.h"
#include "tests/common/robots_str.h"
#include <filesystem>

using namespace contentv1;

TEST_CASE("Test HostDb", "[HostDb]") {
  // std::cout << cfg.help() << std::endl;

  const std::string host_db_path{"/tmp/host_db_test"};
  HostDbConfig cfg;
  cfg[HostDbConfig::STR_HOST_DB_PATH] = host_db_path;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_DAY_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_MINUTE_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_HOUR_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_PARALLEL_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_SYSTEM_PARALLEL_CRAWL] = 1000;

  cfg.onParsed();
  cfg.validate("ut_host_db.cpp");

  std::filesystem::remove_all(host_db_path);
  HostDb db{cfg};

  const std::string expect_robots_url{"https://www.kp.ru/robots.txt"};
  const std::vector<Url> try_urls{"https://www.kp.ru",
                                  "https://www.kp.ru/banners/x/"};
  const Url expect_url_allow{"https://www.kp.ru/"}; // return normalized
  {
    std::vector<Url> allowed_urls;
    std::vector<Url> need_robots;
    auto decisions =
        db.tryCrawl(try_urls, misc::get_now(), allowed_urls, need_robots);

    REQUIRE(allowed_urls.empty());
    REQUIRE(need_robots.size() == 1);
    REQUIRE(need_robots[0] == expect_robots_url);

    REQUIRE(decisions.size() == 2);
    REQUIRE(decisions[0] == HostDb::EHostCrawlDecision::DISALLOW_NEED_ROBOTS);
    REQUIRE(decisions[1] == HostDb::EHostCrawlDecision::DISALLOW_NEED_ROBOTS);
  }

  Url robots_url{"https://www.kp.ru/robots.txt"};
  db.onRobotsTxt(robots_url, g_robots_test_str);
  {
    std::vector<Url> allowed_urls;
    std::vector<Url> need_robots;
    auto decisions =
        db.tryCrawl(try_urls, misc::get_now(), allowed_urls, need_robots);

    for (size_t i = 0; i < decisions.size(); ++i)
      std::cout << decisions[i] << ": " << try_urls[i] << std::endl;
    std::cout << std::endl;

    REQUIRE(decisions.size() == 2);
    REQUIRE(decisions[0] == HostDb::EHostCrawlDecision::ALLOW);
    REQUIRE(decisions[1] == HostDb::EHostCrawlDecision::DISALLOW_BY_ROBOTS);

    REQUIRE(need_robots.empty());
    REQUIRE(allowed_urls.size() == 1);
    REQUIRE(allowed_urls[0] == expect_url_allow);
  }
}

TEST_CASE("Test HostDb restored", "[HostDb]") {
  const std::string host_db_path{"/tmp/host_db_test"};
  HostDbConfig cfg;
  cfg[HostDbConfig::STR_HOST_DB_PATH] = host_db_path;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_DAY_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_MINUTE_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_HOUR_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_HOST_PARALLEL_CRAWL] = 100;
  cfg[CrawlLimitsConfig::INT_MAX_SYSTEM_PARALLEL_CRAWL] = 1000;

  cfg.onParsed();
  cfg.validate("ut_host_db.cpp");
  HostDb db{cfg};
  const Url known_url{"https://www.kp.ru"};
  REQUIRE(db.hasRobots(known_url));
}