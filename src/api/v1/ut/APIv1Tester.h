#pragma once
#include "api/v1/APIv1.h"
#include "worker/ut/QueryLoopTester.h"

namespace contentv1 {
class APIv1Tester {

  QueryLoopTester::State m_query_loop_state;

  inline static const std::string s_test_addr{"127.0.0.1"};
  inline static const int s_test_port{8083};

  APIv1::Ptr m_api;
  std::string m_key_pair;

public:
  APIv1Tester();

private:
  void _setUp();
  void _testSimpleCall();
  std::string _testOAuth2GetToken();
  void _testOAuth2();
  void _testListQG(const std::vector<QueryGroup> &_qgs_expect);

  void _testSetGetQG(const std::string &_token,
                     const QueryGroup &_query_group_expect);
  void _testSetGetQG(const std::vector<QueryGroup> &_qgs_expect);

  void _testHits(const std::vector<QueryGroup> &_qgs,
                 const std::vector<TestPage> &_pages);
  void _testHitsImpl(const std::string &_token,
                     const std::vector<QueryGroup> &_qgs,
                     const std::vector<TestPage> &_pages);
  void _getHitsAndCompare(const std::string &_token, const GroupId &_gid,
                          const QueryGroupHits &_hits_expect);
  static void _fillPages(IPageDb::Ptr _page_db,
                         const std::vector<TestPage> &_pages);
};
} // namespace contentv1
