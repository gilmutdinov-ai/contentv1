#pragma once

#include "core/CrawlLimits.h"
#include "core/TpSlidWin.h"

namespace contentv1 {

class HostFreq {
  Cnt m_crawling_now = 0;

  TpSlidWin m_min{std::chrono::minutes{1}};
  TpSlidWin m_hour{std::chrono::hours{1}};
  TpSlidWin m_day{std::chrono::days{1}};

public:
  bool isAllowed(const CrawlLimits &_limits);
  void incCrawling();
  void decCrawling();
};

} // namespace contentv1