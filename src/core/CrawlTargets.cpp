#include "core/CrawlTargets.h"

namespace contentv1 {

CrawlTargetsConfig::CrawlTargetsConfig() { addInts(s_cfg_ints); }

CrawlTargets::CrawlTargets(const CrawlTargetsConfig &_config)
    : target_day_crawl{
          _config[CrawlTargetsConfig::INT_TARGET_DAY_CRAWL].asInt()} {}

} // namespace contentv1