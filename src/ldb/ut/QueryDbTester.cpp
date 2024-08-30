#include "QueryDbTester.h"

namespace contentv1 {

static QueryGroup genQueryGroup(const std::string &_gid, size_t _size) {
  QueryGroup g1;
  g1.gid = _gid;
  for (size_t i = 0; i < _size; ++i)
    g1.queries.push_back("query_" + std::to_string(i));
  return g1;
}

static QueryGroupHits genQueryGroupHits(const std::string &_gid, size_t _size) {
  QueryGroupHits h1;
  h1.gid = _gid;
  for (size_t i = 0; i < _size; ++i)
    h1.hits.push_back("url_" + std::to_string(i));
  return h1;
}

static void testQueryGroup() {
  {
    QueryGroup g1 = genQueryGroup("g1", 10);
    QueryGroup g1_restored{g1.dump()};
    REQUIRE(g1 == g1_restored);
  }
  {
    QueryGroupHits h1 = genQueryGroupHits("g1", 10);
    QueryGroupHits h1_restored{h1.dump()};
    REQUIRE(h1 == h1_restored);
  }
}

static void testQueryDb() {

  const size_t qg_count{19};
  const size_t qg_hits_count{17};

  const std::string db_path{"/tmp/db_ut/query_db"};
  std::filesystem::remove_all(db_path);

  QueryDbConfig cfg;
  cfg[QueryDbConfig::STR_QUERY_DB_PATH] = db_path;
  cfg.onParsed();
  cfg.validate("QueryDbTester::QueryDbTester");

  QueryDb db{cfg};

  for (size_t i = 0; i < qg_count; ++i) {
    QueryGroup g1 = genQueryGroup("g" + std::to_string(i), i);
    db.saveQG(g1);
    QueryGroup g1_loaded;
    REQUIRE(db.loadQG(g1.gid, g1_loaded));
    REQUIRE(g1 == g1_loaded);
  }

  for (size_t i = 0; i < qg_hits_count; ++i) {
    QueryGroupHits h1 =
        genQueryGroupHits("g" + std::to_string(i), 2 + qg_hits_count - i);
    db.saveQGHits(h1);
    QueryGroupHits h1_loaded;
    REQUIRE(db.loadQGHits(h1.gid, h1_loaded));
    REQUIRE(h1 == h1_loaded);
  }
  {
    std::vector<QueryGroup> qg_all;
    db.getAllQG(qg_all);
    REQUIRE(qg_all.size() == qg_count);
  }
  {
    std::vector<QueryGroupHits> qg_hits_all;
    db.getAllQGHits(qg_hits_all);
    REQUIRE(qg_hits_all.size() == qg_hits_count);
  }
  {
    GroupId gid{"g0"};
    db.deleteQG(gid);
    std::vector<QueryGroup> qg_all;
    db.getAllQG(qg_all);
    REQUIRE(qg_all.size() == qg_count - 1);
  }
  {
    GroupId gid{"g0"};
    db.deleteQGHits(gid);
    std::vector<QueryGroupHits> qg_hits_all;
    db.getAllQGHits(qg_hits_all);
    REQUIRE(qg_hits_all.size() == qg_hits_count - 1);
  }
}

QueryDbTester::QueryDbTester() {

  testQueryGroup();
  testQueryDb();
}

} // namespace contentv1
