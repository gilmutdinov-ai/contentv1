#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "core/UrlFreqMap.h"
#include "tests/common/test_visits.h"

using namespace contentv1;

TEST_CASE("Test UrlFreqMap", "[UrlFreqMap]") {

  UrlFreqMap freq_map{std::chrono::days{7}};

  auto now = std::chrono::system_clock::now();
  Url url{"https://ya.ru"};

  test_visits(url, now,
              [&](const UrlVisit &_uv) { freq_map.insert(_uv, now); });

  Url url2{"https://mail.ru"};
  {
    UrlVisit v(url2, now - std::chrono::days{3});
    freq_map.insert(v, now);
  }

  {
    UrlFreqMap freq_map_merged{std::chrono::days{7}};
    freq_map_merged.merge(freq_map);

    std::vector<UrlFreq> top_urls;
    freq_map_merged.getTopUrls(top_urls);

    /*  for (size_t i = 0; i < std::min(static_cast<size_t>(10),
      top_urls.size());
           ++i) {
        std::cout << top_urls[i].second << " " << top_urls[i].first <<
      std::endl;
      }
      */
    REQUIRE(!top_urls.empty());
    REQUIRE(top_urls[0].first == url);
    REQUIRE(top_urls[1].first == url2);
    REQUIRE(freq_map_merged.checkSortedAndDestroyQueue());
  }
}
