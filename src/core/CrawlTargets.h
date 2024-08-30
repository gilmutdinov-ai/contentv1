#pragma once

#include "core/Types.h"
#include "misc/Config.h"

namespace contentv1 {

class CrawlTargetsConfig : virtual public misc::Config {
public:
  inline static const std::string INT_TARGET_DAY_CRAWL{
      "sched_target_day_crawl"};

private:
  const std::vector<std::string> s_cfg_ints = {INT_TARGET_DAY_CRAWL};

public:
  CrawlTargetsConfig();
};

struct CrawlTargets {

  //  Cnt target_hour_crawl;
  Cnt target_day_crawl;

  CrawlTargets(const CrawlTargetsConfig &_config);
};

} // namespace contentv1