#include <catch2/catch_test_macros.hpp>

#include "ldb/UrlsDaysDb.h"
#include <filesystem>
#include <iostream>

using namespace contentv1;

TEST_CASE("Test UrlsDaysDb", "[UrlsDaysDb]") {

  const std::string ud_db_path{"/tmp/test_ud_db"};

  std::filesystem::remove_all(
      ud_db_path); // events from previous tests saved :-)

  UrlsDaysDbConfig cfg;
  cfg[UrlsDaysDbConfig::STR_URLS_DAYS_DB_PATH] = ud_db_path;
  cfg[UrlsDaysDbConfig::INT_MERGE_INTERVAL_SECS] = 1;
  cfg[UrlsDaysDbConfig::INT_SNAPSHOT_INTERVAL_SECS] = 2;

  cfg.onParsed();
  cfg.validate("ut_urls_days_db.cpp");

  Url url = {"https://ya.ru"};
  {
    UrlsDaysDb db(cfg);

    auto now = misc::get_now();
    for (size_t i = 30; i > 0; --i) {
      UrlVisit v(url, now - std::chrono::days{i});
      db.saveVisit(v);
      if (i < 7)
        db.saveVisit(v);
    }

    sleep(cfg[UrlsDaysDbConfig::INT_MERGE_INTERVAL_SECS].as<int>() +
          cfg[UrlsDaysDbConfig::INT_SNAPSHOT_INTERVAL_SECS].as<int>() + 1);
    // std::cout << "db size: " << db.size() << std::endl;
    //  db.print();
  }
  {
    // load saved db
    UrlsDaysDb db(cfg);
    // db.print();

    REQUIRE(db.size() == 1);
    std::vector<UrlTpStat> utsv;
    REQUIRE(db.getUTSV(url, utsv));
    // std::cout << "utsv.size: " << utsv.size() << std::endl;
    REQUIRE(utsv.size() == 12);
    REQUIRE(utsv[0].cnt == 1);
    REQUIRE(utsv[11].cnt == 2);
  }
}
