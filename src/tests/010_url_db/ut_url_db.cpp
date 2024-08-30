#include <catch2/catch_test_macros.hpp>

#include "ldb/UrlDb.h"
#include "misc/Time.h"
#include <filesystem>

using namespace contentv1;

TEST_CASE("Test UrlDb", "[UrlDb]") {

  const std::string url_db_path = {"/tmp/url_db_test"};
  const int url_db_snapshot_interval_sec{2};

  const auto now = misc::get_now();

  const Url url{"https://www.ya.ru/"};

  UrlDbConfig cfg;
  cfg[UrlDbConfig::STR_URL_DB_PATH] = url_db_path;
  cfg[UrlDbConfig::INT_URL_DB_SNAPSHOT_INTERVAL_SEC] =
      url_db_snapshot_interval_sec;

  cfg.onParsed();
  cfg.validate("ut_url_db.cpp");

  std::filesystem::remove_all(url_db_path);
  {
    UrlDb db{cfg};

    db.setEnqueued({url}, now);

    CrawlAttempts cas;
    REQUIRE(db.getAttempts(url, cas));
    REQUIRE(cas.size() == 1);

    auto ca = cas.getLast();
    REQUIRE(misc::get_day_time_str(ca.tp) == misc::get_day_time_str(now));
    REQUIRE(ca.status == CrawlAttemptsPb::ATTEMPT_STATUS_ENQUEUED);
  }
  // check restored
  {
    UrlDb db{cfg};
    CrawlAttempts cas;
    REQUIRE(db.getAttempts(url, cas));
    REQUIRE(cas.size() == 1);

    auto ca = cas.getLast();
    REQUIRE(misc::get_day_str(ca.tp) ==
            misc::get_day_str(now)); // tp == bad like float
    REQUIRE(ca.status == CrawlAttemptsPb::ATTEMPT_STATUS_ENQUEUED);

    // check two attempts
  }
}
