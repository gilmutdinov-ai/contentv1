#include <catch2/catch_test_macros.hpp>

#include "ldb/UrlsDays.h"
#include <iostream>

using namespace contentv1;

TEST_CASE("Test UrlsDays", "[UrlsDays]") {

  UrlsDays<14> ud;
  UrlsDays<14> ud2;
  Url url = {"https://ya.ru"};
  auto now = misc::get_now();
  for (size_t i = 30; i > 0; --i) {
    UrlVisit v(url, now - std::chrono::days{i});
    ud.add(v, now);
  }
  for (size_t i = 7; i > 0; --i) {
    UrlVisit v(url, now - std::chrono::days{i});
    ud2.add(v, now);
  }

  ud2.merge(ud, now);

  UrlsDays<14> ud3(ud2.dump());
  // ud3.print();

  REQUIRE(ud3.size() == 1);
  std::vector<UrlTpStat> utsv;
  REQUIRE(ud3.getUTSV(url, utsv));
  // check size
  REQUIRE(utsv.size() == 13);
  // check order
  REQUIRE(utsv[0].cnt == 1);
  REQUIRE(utsv[12].cnt == 2);

  // std::cout << "utsv.size: " << utsv.size() << std::endl;
}
