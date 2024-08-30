#pragma once

#include "core/FetchResult.h"
#include "core/HostFreq.h"
#include "core/Robots.h"
#include "core/Types.h"
#include "core/UrlParsed.h"
#include "leveldb/db.h"
#include "misc/Config.h"
#include <cassert>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

/*
    Перед любой попыткой что-то скачать, проиходит обращение к единственному в
   системе инстансу HostDb, он либо разрешает либо нет
*/

namespace contentv1 {

class HostDbConfig : virtual public CrawlLimitsConfig {
public:
  inline static const std::string STR_HOST_DB_PATH{"sched_host_db_path"};

private:
  const std::vector<std::string> s_cfg_strings = {STR_HOST_DB_PATH};

public:
  HostDbConfig();
};

class HostDb {

  std::mutex m_mtx;

  // limits
  CrawlLimits m_limits;

  // not persistent
  std::unordered_map<Host, HostFreq> m_host_freq;
  HostFreq m_global_freq;
  // persistent
  std::unordered_map<Host, Robots> m_host_robots;
  leveldb::DB *m_db;

public:
  enum EHostCrawlDecision {
    ALLOW = 1,
    DISALLOW_NEED_ROBOTS = 2,
    DISALLOW_BY_ROBOTS = 3,
    DISALLOW_URL_PARSING = 4,
    DISALLOW_FREQ = 5
  };

  using Ptr = std::shared_ptr<HostDb>;

  HostDb(const HostDbConfig &_config);
  ~HostDb();

  void leaveAllowedByRobots(const std::vector<UrlFreq> &_urls,
                            std::vector<UrlFreq> &_allowed_urls,
                            std::vector<UrlFreq> &_need_robots);

  // allowed_urls normalized but not exactly unique
  // return decisions corresponding to _try_urls
  // increment internal freq when allowed
  std::vector<EHostCrawlDecision> tryCrawl(const std::vector<Url> &_try_urls,
                                           Tp now,
                                           std::vector<Url> &_allowed_urls,
                                           std::vector<Url> &_need_robots);

  std::vector<EHostCrawlDecision>
  tryCrawl(const std::vector<GFQRecord> &_try_urls, Tp now,
           std::vector<GFQRecord> &_allowed_urls,
           std::vector<Url> &_need_robots);

  // void onCrawled(const std::vector<Url> &_urls);
  void onFetched(const std::vector<FetchResult> &_results);

  // for tests only!
  void onRobotsTxt(const Url &_robots_url, const std::string &_robots_str);

  // READ
  bool hasRobots(const Url &url);
  Cnt sizeRobots();

private:
  EHostCrawlDecision _tryCrawl(const UrlParsed &_url_parsed, Url &need_robots);
  // если урл и есть роботс, то разрешено, иначе по базе
  bool _isAllowedRobots(const UrlParsed &_url_parsed, Url &need_robots) const;
  bool _isAllowedFreq(const Host &_host);
  void _setCrawling(const Host &_host);

  void _loadRobotsDb();
};
} // namespace contentv1