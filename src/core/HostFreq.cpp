#include "HostFreq.h"

namespace contentv1 {

bool HostFreq::isAllowed(const CrawlLimits &_limits) {

  if (m_crawling_now >= _limits.max_host_parallel_crawl)
    return false;

  if (m_min.size() >= _limits.max_host_minute_crawl)
    return false;

  if (m_hour.size() >= _limits.max_host_hour_crawl)
    return false;

  if (m_day.size() >= _limits.max_host_day_crawl)
    return false;

  return true;
}

void HostFreq::incCrawling() {
  ++m_crawling_now;
  m_min.inc();
  m_hour.inc();
  m_day.inc();
}

void HostFreq::decCrawling() {
  if (m_crawling_now <= 0)
    return;
  --m_crawling_now;
}
} // namespace contentv1
