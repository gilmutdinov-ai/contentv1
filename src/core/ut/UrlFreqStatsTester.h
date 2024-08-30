#pragma once

#include "core/UrlFreqStatsLF.h"
#include "tests/common/test_visits.h"
#include <catch2/catch_test_macros.hpp>
#include <type_traits>

namespace contentv1 {

template <typename T> static void checkWait(T &_freq_stats) {
  if constexpr (std::is_same_v<T, UrlFreqStatsLF>) {
    _freq_stats.waitMerged();
  }
}

template <typename UFS> class UrlFreqStatsTester {
public:
  UrlFreqStatsTester() {
    UFS freq_stats;
    auto now = std::chrono::system_clock::now();

    Url url{"https://ya.ru"};

    test_visits(url, now,
                [&](const UrlVisit &_uv) { freq_stats.insert(_uv, now); });

    Url url2{"https://mail.ru"};
    {
      UrlVisit v(url2, now - std::chrono::days{1});
      freq_stats.insert(v, now);
    }

    checkWait<UFS>(freq_stats);

    std::vector<UrlFreq> top_urls;
    freq_stats.getTopUrls(top_urls);

    for (size_t i = 0; i < std::min(static_cast<size_t>(10), top_urls.size());
         ++i) {
      std::cout << top_urls[i].second << " " << top_urls[i].first << std::endl;
    }

    REQUIRE(!top_urls.empty());
    REQUIRE(top_urls[0].first == url);
    REQUIRE(top_urls[1].first == url2);
  }
};
} // namespace contentv1