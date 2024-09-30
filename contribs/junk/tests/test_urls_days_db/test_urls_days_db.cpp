#include "db/UrlsDaysDb.h"
#include <filesystem>
#include <iostream>

using namespace contentv1;

int main() {

  const std::string ud_db_path{"/tmp/test_ud_db"};

  std::filesystem::remove_all(
      ud_db_path); // events from previous tests saved :-)

  UrlsDaysDbConfig cfg;
  cfg.parseJsonString("{\"urls_days_db_path\":\"" + ud_db_path +
                      "\", "
                      "\"snapshot_interval_secs\":2}");
  cfg.validate();

  Url url = {"https://ya.ru"};
  {
    UrlsDaysDb db(cfg);

    auto now = get_now();
    for (size_t i = 30; i > 0; --i) {
      UrlVisit v(url, now - std::chrono::days{i});
      db.saveVisit(v);
      if (i < 7)
        db.saveVisit(v);
    }

    sleep(UrlsDaysDb::MERGE_WAIT_SECS +
          cfg[UrlsDaysDbConfig::INT_SNAPSHOT_INTERVAL_SECS].asInt() + 1);
    // std::cout << "db size: " << db.size() << std::endl;
    //  db.print();
  }
  {
    // load saved db
    UrlsDaysDb db(cfg);
    // db.print();

    assert(db.size() == 1);
    std::vector<UrlTpStat> utsv;
    REQUIRE(db.getUTSV(url, utsv));
    // std::cout << "utsv.size: " << utsv.size() << std::endl;
    assert(utsv.size() == 12);
    assert(utsv[0].cnt == 1);
    assert(utsv[11].cnt == 2);
  }
  return 0;
}
