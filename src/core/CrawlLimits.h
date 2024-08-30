#pragma once

#include "core/Types.h"
#include "misc/Config.h"

namespace contentv1 {

class CrawlLimitsConfig : virtual public misc::Config {
public:
  inline static const std::string INT_MAX_HOST_PARALLEL_CRAWL{
      "sched_max_host_parallel_crawl"};
  inline static const std::string INT_MAX_HOST_MINUTE_CRAWL{
      "sched_max_host_minute_crawl"};
  inline static const std::string INT_MAX_HOST_HOUR_CRAWL{
      "sched_max_host_hour_crawl"};
  inline static const std::string INT_MAX_HOST_DAY_CRAWL{
      "sched_max_host_day_crawl"};
  inline static const std::string INT_MAX_SYSTEM_PARALLEL_CRAWL{
      "sched_max_system_parallel_crawl"};

private:
  const std::vector<std::string> s_cfg_ints = {
      INT_MAX_HOST_PARALLEL_CRAWL, INT_MAX_HOST_MINUTE_CRAWL,
      INT_MAX_HOST_HOUR_CRAWL, INT_MAX_HOST_DAY_CRAWL,
      INT_MAX_SYSTEM_PARALLEL_CRAWL};

public:
  CrawlLimitsConfig() { addInts(s_cfg_ints); }
};

struct CrawlLimits {

  Cnt max_host_parallel_crawl;
  Cnt max_host_minute_crawl;
  Cnt max_host_hour_crawl;
  Cnt max_host_day_crawl;
  Cnt max_system_parallel_crawl;

  CrawlLimits(const CrawlLimitsConfig &_config);
};

} // namespace contentv1