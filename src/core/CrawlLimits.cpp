#include "core/CrawlLimits.h"

namespace contentv1 {

CrawlLimitsConfig::CrawlLimitsConfig() { addInts(s_cfg_ints); }

CrawlLimits::CrawlLimits(const CrawlLimitsConfig &_cfg) {
  max_host_parallel_crawl = _cfg["sched_max_host_parallel_crawl"].as<int>();
  max_host_minute_crawl = _cfg["sched_max_host_minute_crawl"].as<int>();
  max_host_hour_crawl = _cfg["sched_max_host_hour_crawl"].as<int>();
  max_host_day_crawl = _cfg["sched_max_host_day_crawl"].as<int>();
  max_system_parallel_crawl = _cfg["sched_max_system_parallel_crawl"].as<int>();
}
} // namespace contentv1
