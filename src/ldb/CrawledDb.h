#pragma once
#include "core/FetchResult.h"
#include "ldb/UrlsDaysDb.h"
#include "src/proto/CrawlAttempts.pb.h"

namespace contentv1 {

/*

Хранить по дням результаты скачек, внутри инстанс UrlsDaysDb

  //   ATTEMPT_STATUS_ENQUEUED = 0, -  не декрементится когда PENDING
  //   ATTEMPT_STATUS_PENDING = 1,
  Все остальное FINISHED

*/

class CrawledDbConfig : virtual public misc::Config {
public:
  inline static const std::string STR_CRAWLED_DB_PATH{"sched_crawled_db_path"};

  inline static const std::string INT_CRAWLED_DB_MERGE_INTERVAL_SECS{
      "sched_crawled_db_merge_interval_secs"};
  inline static const std::string INT_CRAWLED_DB_SNAPSHOT_INTERVAL_SECS{
      "sched_crawled_db_snapshot_interval_secs"};

private:
  const std::vector<std::string> s_cfg_strs{STR_CRAWLED_DB_PATH};
  const std::vector<std::string> s_cfg_ints{
      INT_CRAWLED_DB_SNAPSHOT_INTERVAL_SECS};

public:
  CrawledDbConfig();
  virtual ~CrawledDbConfig() = default;
};

class CrawledDb {
  std::unique_ptr<UrlsDaysDb> m_db;

public:
  using Ptr = std::shared_ptr<CrawledDb>;
  CrawledDb(const CrawledDbConfig &_config);

  void onFetched(const std::vector<FetchResult> &_results);

  void saveResult(CrawlAttemptsPb::AttemptStatus _result);
  void saveResult(CrawlAttemptsPb::AttemptStatus _result, Cnt _cnt);
  Cnt getFinishedToday();
  Cnt getInProgress();

private:
  std::vector<int> m_enum_finished_values;

  static Url genFakeUrlForResult(int _value_id);
  void _fillEnumFinishedValues();
  Cnt _getTodayEvents(int _event_id);
};

} // namespace contentv1