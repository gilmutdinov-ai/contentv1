#pragma once
#include "tests/common/test_pages.h"
#include "tests/common/test_qgs.h"
#include "worker/QueryLoop.h"

namespace contentv1 {

class QueryLoopTester {

  QueryDb::Ptr m_query_db;
  IPageDb::Ptr m_page_db;
  QueryLoop::Ptr m_query_loop;

public:
  struct State {
    QueryDb::Ptr query_db;
    IPageDb::Ptr page_db;
    QueryLoop::Ptr query_loop;
  };
  static State createCleanState(const std::string &_ut_data_path);

  QueryLoopTester();

private:
  void setUp();
  void clean();
  void testQueryResult(const std::vector<QueryGroupHits> &_qgs_hits,
                       std::unordered_map<Url, TestPage> &_pages_map) const;

  void testQueryBase();
  void testWithQueryDb();
};

} // namespace contentv1