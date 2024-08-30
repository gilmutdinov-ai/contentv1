#pragma once

#include "core/CrawlAttempts.h"
#include "core/CrawlTargets.h"
#include "core/FetchResult.h"
#include "core/Types.h"
#include "leveldb/db.h"
#include "misc/Config.h"
#include "src/proto/UrlDb.pb.h"
#include <atomic>
#include <memory>
#include <string>
#include <thread>

namespace contentv1 {

class UrlDbConfig : virtual public misc::Config {
public:
  inline static const std::string STR_URL_DB_PATH{"sched_url_db_path"};
  inline static const std::string INT_URL_DB_SNAPSHOT_INTERVAL_SEC{
      "sched_url_db_snapshot_interval_sec"};

protected:
  const std::vector<std::string> s_cfg_strings = {STR_URL_DB_PATH};
  const std::vector<std::string> s_cfg_ints = {
      INT_URL_DB_SNAPSHOT_INTERVAL_SEC};

public:
  UrlDbConfig();
};

/*
10 последних попыток скачать

  crawl policy

    EnqeueLoop берет верхушку посещаемых и обращается к UrlDb нужно ли качать

        ATTEMPT_STATUS_ENQUEUED = 0;
        ATTEMPT_STATUS_PENDING = 1;
        ATTEMPT_STATUS_OK = 2;
        ATTEMPT_STATUS_CURL_ERR = 3;
        ATTEMPT_STATUS_4xx = 4;
        ATTEMPT_STATUS_5xx = 5;

    need_crawl? add to global queue
      never attempted - yes
      last attempt CURL_ERR - try in hour
      last attempt 4xx or 5xx - wait x2 each time
      last attempted enqueued - wait 1 day
      last attempt pending - wait 1 hour
      last attempt succeded - wait 1 week
*/

class UrlDb {
public:
private:
  std::mutex m_mtx;

  std::unordered_map<Url, CrawlAttempts> m_url_crawl_attempts;
  leveldb::DB *m_attempts_db;

  // async flush
  std::atomic<bool> m_running = true;
  std::unique_ptr<std::thread> m_snap_th;
  const int m_snapshot_interval_sec;

private:
  void _loadData(const std::string &_url_db_attempts_path);
  void _loadCrawlAttempts(const std::string &_url_db_attempts_path);

  void _snapThread();
  void _snapData();

  EUrlCrawlDecisionPb _IsNeedCrawl(const Url &_url, Tp _now) const;
  EUrlCrawlDecisionPb _IsNeedCrawlRobots(const Url &_robots_url, Tp _now) const;

  void _setEnqueued(const Url &_url, Tp tp);
  void _setStatus(const FetchResult &_result, Tp _now);

  void setResult(const Url &_url, CrawlAttemptsPb::AttemptStatus _result,
                 Tp now);

public:
  using Ptr = std::shared_ptr<UrlDb>;
  UrlDb(const UrlDbConfig &_config);
  ~UrlDb();

  // WRITE
  void setEnqueued(const std::vector<Url> &in_urls, Tp now);
  void onFetched(const std::vector<FetchResult> &_results);

  // READ
  // what is crawlable according to policy, const if not mutex
  std::vector<EUrlCrawlDecisionPb>
  leaveNeedUrls(const std::vector<UrlFreq> &in_urls, Tp now,
                std::vector<UrlFreq> &out_urls);

  // robots: what is crawlable according to robots policy, const if not mutex
  std::vector<EUrlCrawlDecisionPb>
  leaveNeedRobots(const std::vector<UrlFreq> &_in_robots, Tp _now,
                  std::vector<UrlFreq> &_need_robots);

  bool getAttempts(const Url &_url, CrawlAttempts &_ca);
  Cnt size();
};
} // namespace contentv1