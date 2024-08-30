#pragma once
#include "core/QueryGroup.h"
#include "ldb/PageDb.h"
#include "ldb/QueryDb.h"
#include "misc/Config.h"
#include <atomic>
#include <memory>

namespace contentv1 {

class QueryLoopConfig : virtual public QueryDbConfig {
public:
  inline static const std::string INT_QUERY_INTERVAL_SEC{"query_interval_sec"};

  QueryLoopConfig();
};

class QueryLoop {
  QueryDb::Ptr m_query_db;
  IPageDb::Ptr m_page_db;

  std::atomic<bool> m_running{true};
  int m_query_interval_sec;

  std::atomic<int64_t> m_cycles_count{0};

public:
  using Ptr = std::shared_ptr<QueryLoop>;
  QueryLoop(QueryDb::Ptr _query_db, IPageDb::Ptr _page_db,
            const QueryLoopConfig &_config);

  void loop();
  void stop();

  void waitNextCycle();

private:
  void _loopImpl();
  void _query(const std::vector<QueryGroup> &_qg,
              std::vector<QueryGroupHits> &_qg_hits);

  friend class QueryLoopTester;
};
} // namespace contentv1