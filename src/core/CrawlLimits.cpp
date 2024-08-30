#include "core/CrawlLimits.h"

namespace contentv1 {

CrawlLimits::CrawlLimits(const CrawlLimitsConfig &_cfg) {
  max_host_parallel_crawl = _cfg["sched_max_host_parallel_crawl"].asInt();
  max_host_minute_crawl = _cfg["sched_max_host_minute_crawl"].asInt();
  max_host_hour_crawl = _cfg["sched_max_host_hour_crawl"].asInt();
  max_host_day_crawl = _cfg["sched_max_host_day_crawl"].asInt();
  max_system_parallel_crawl = _cfg["sched_max_system_parallel_crawl"].asInt();
}
} // namespace contentv1
