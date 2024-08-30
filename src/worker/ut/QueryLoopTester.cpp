#include "worker/ut/QueryLoopTester.h"
#include "core/QueryGroup.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <string>

namespace contentv1 {

QueryLoopTester::State
QueryLoopTester::createCleanState(const std::string &_ut_data_path) {

  QueryDb::Ptr query_db;
  IPageDb::Ptr page_db;
  QueryLoop::Ptr query_loop;

  std::filesystem::remove_all(_ut_data_path);
  {
    QueryDbConfig cfg;
    cfg[QueryDbConfig::STR_QUERY_DB_PATH] = _ut_data_path + "/query_db";
    cfg.onParsed();
    cfg.validate("QueryLoopTester::createCleanState");
    query_db.reset(new QueryDb{cfg});
  }
  {
    PageDbConfig cfg;
    cfg[PageDbConfig::STR_PAGE_DB_PATH] = _ut_data_path + "/page_db";
    cfg.onParsed();
    cfg.validate("QueryLoopTester::createCleanState");
    page_db.reset(new PageDb{cfg});
  }
  {
    QueryLoopConfig cfg;
    cfg[QueryDbConfig::STR_QUERY_DB_PATH] = _ut_data_path + "/query_db";
    cfg[QueryLoopConfig::INT_QUERY_INTERVAL_SEC] = 2;
    cfg.onParsed();
    cfg.validate("QueryLoopTester::createCleanState");
    query_loop.reset(new QueryLoop{query_db, page_db, cfg});
  }
  return QueryLoopTester::State{query_db, page_db, query_loop};
}

void QueryLoopTester::setUp() {

  // for multiple tests
  m_query_db.reset();
  m_page_db.reset();
  m_query_loop.reset();

  const std::string ut_path{"/tmp/query_loop_ut/"};
  auto state = QueryLoopTester::createCleanState(ut_path);

  m_query_db = state.query_db;
  m_page_db = state.page_db;
  m_query_loop = state.query_loop;
  /*
    std::filesystem::remove_all(ut_path);
    {
      QueryDbConfig cfg;
      cfg[QueryDbConfig::STR_QUERY_DB_PATH] = ut_path + "query_db";
      cfg.onParsed();
      cfg.validate("QueryLoopTester::testQueryBase");
      m_query_db.reset(new QueryDb{cfg});
    }
    {
      PageDbConfig cfg;
      cfg[PageDbConfig::STR_PAGE_DB_PATH] = ut_path + "page_db";
      cfg.onParsed();
      cfg.validate("QueryLoopTester::testQueryBase");
      m_page_db.reset(new PageDb{cfg});
    }
    {
      QueryLoopConfig cfg;
      cfg[QueryDbConfig::STR_QUERY_DB_PATH] = ut_path + "query_db";
      cfg[QueryLoopConfig::INT_QUERY_INTERVAL_SEC] = 2;
      cfg.onParsed();
      cfg.validate("QueryLoopTester::testQueryBase");
      m_query_loop.reset(new QueryLoop{m_query_db, m_page_db, cfg});
    }
    */
}

void QueryLoopTester::testQueryResult(
    const std::vector<QueryGroupHits> &_qgs_hits,
    std::unordered_map<Url, TestPage> &_pages_map) const {

  REQUIRE(!_qgs_hits.empty());
  for (auto qg_hits : _qgs_hits) {
    auto gid = qg_hits.gid;
    for (auto found_url : qg_hits.hits) {
      const auto &doc_groups = _pages_map[found_url].groups;
      if (!doc_groups.empty())
        REQUIRE(doc_groups.find(gid) != doc_groups.end());
    }
  }
}

void QueryLoopTester::testQueryBase() {

  setUp();

  auto qgs = test_qgs();
  auto pages = test_pages();
  auto pages_map = build_pages_map(pages);
  for (const auto &page : pages)
    m_page_db->save(page.url, page.content);

  // query
  std::vector<QueryGroupHits> qgs_hits;
  m_query_loop->_query(qgs, qgs_hits);

  testQueryResult(qgs_hits, pages_map);
}

void QueryLoopTester::testWithQueryDb() {

  setUp();
  auto qgs = test_qgs();
  auto pages = test_pages();
  auto pages_map = build_pages_map(pages);
  for (const auto &page : pages)
    m_page_db->save(page.url, page.content);

  // save queries to db
  for (auto qg : qgs)
    m_query_db->saveQG(qg);

  // OUR PATIENT
  m_query_loop->_loopImpl();

  // load hits from db
  std::vector<QueryGroupHits> qgs_hits;
  m_query_db->getAllQGHits(qgs_hits);

  // test
  testQueryResult(qgs_hits, pages_map);
}

QueryLoopTester::QueryLoopTester() {
  testQueryBase();
  testWithQueryDb();
}

} // namespace contentv1