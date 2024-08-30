#include "api/v1/ut/APIv1Tester.h"
#include "worker/ut/QueryLoopTester.h"
#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <stdexcept>

namespace contentv1 {

void APIv1Tester::_setUp() {

  {
    std::stringstream ss;
    ss << APIv1::s_api_keys_hardcode[0] << ":" << APIv1::s_api_keys_hardcode[1];
    m_key_pair = ss.str();
  }

  const std::string ut_path{"/tmp/apiv1_ut/"};
  m_query_loop_state = QueryLoopTester::createCleanState(ut_path);

  APIv1Config cfg;
  cfg[APIv1Config::STR_LISTEN_ADDR] = s_test_addr;
  cfg[APIv1Config::INT_LISTEN_PORT] = s_test_port;
  m_api.reset(new APIv1{m_query_loop_state.query_db,
                        m_query_loop_state.query_loop, cfg});
  sleep(2);
}

void APIv1Tester::_fillPages(IPageDb::Ptr _page_db,
                             const std::vector<TestPage> &_pages) {

  for (const auto &page : _pages)
    _page_db->save(page.url, page.content);
}

void APIv1Tester::_testSimpleCall() {

  httplib::Client cli{"http://" + s_test_addr + ":" +
                      std::to_string(s_test_port)};
  auto res = cli.Get("/version");
  REQUIRE(res->status == 200);
  REQUIRE(res->body == APIv1::s_version_json_str);
}

std::string APIv1Tester::_testOAuth2GetToken() {

  httplib::Headers headers;
  headers.insert(std::make_pair(
      "Authorization", "Basic " + httplib::detail::base64_encode(m_key_pair)));

  httplib::Client cli{"http://" + s_test_addr + ":" +
                      std::to_string(s_test_port)};
  auto res = cli.Get("/token", headers);

  REQUIRE(res->status == 200);

  Json::Value js;
  REQUIRE(Json::Reader().parse(res->body, js));
  REQUIRE(js["access_token"].isString());

  std::string token = js["access_token"].asString();
  REQUIRE(APIv1::_isToken(token));
  return token;
}

void APIv1Tester::_testOAuth2() {

  {
    std::stringstream ss;
    ss << "curl -u \"";
    //_API_KEY:_API_SECRET_KEY
    ss << m_key_pair;
    ss << "\" http://";
    ss << s_test_addr << ":" << s_test_port << "/token";
    auto token_url_cmd = ss.str();
    std::cout << "Simulating curl command: " << token_url_cmd << std::endl;
    // system(token_url_cmd.c_str());
  }
  // token call
  std::string token = _testOAuth2GetToken();
  // check_token call
  {
    httplib::Headers headers;
    headers.insert(std::make_pair("Authorization", "Bearer " + token));

    httplib::Client cli{"http://" + s_test_addr + ":" +
                        std::to_string(s_test_port)};
    auto res = cli.Get("/check_token", headers);

    REQUIRE(res->status == 200);

    Json::Value js;
    REQUIRE(Json::Reader().parse(res->body, js));
    REQUIRE(js["status"].isString());
    REQUIRE(js["status"].asString() == "ok");
  }
  // check_token with bad token
  {
    httplib::Headers headers;
    headers.insert(std::make_pair("Authorization", "Bearer _something"));

    httplib::Client cli{"http://" + s_test_addr + ":" +
                        std::to_string(s_test_port)};
    auto res = cli.Get("/check_token", headers);

    REQUIRE(res->status == 403);

    Json::Value js;
    REQUIRE(Json::Reader().parse(res->body, js));
    REQUIRE(js["status"].isString());
    REQUIRE(js["status"].asString() == "denied");
  }
  // REQUIRE(1 == 2);
}

void APIv1Tester::_testListQG(const std::vector<QueryGroup> &_qgs_expect) {

  std::string token = _testOAuth2GetToken();

  httplib::Headers headers;
  headers.insert(std::make_pair("Authorization", "Bearer " + token));

  httplib::Client cli{"http://" + s_test_addr + ":" +
                      std::to_string(s_test_port)};
  auto res = cli.Get("/get_query_groups_list", headers);
  REQUIRE(res->status == 200);

  std::cout << res->body << std::endl;

  Json::Value js;
  REQUIRE(Json::Reader().parse(res->body, js));
  REQUIRE(js["status"].isString());
  REQUIRE(js["status"].asString() == "ok");
  REQUIRE(js["query_groups_count"].isInt());
  int groups_count = js["query_groups_count"].asInt();
  REQUIRE(groups_count == _qgs_expect.size());
  if (_qgs_expect.size() == 0)
    return;

  REQUIRE(js["query_groups"].isArray());
  REQUIRE(_qgs_expect.size() == js["query_groups"].size());

  std::set<GroupId> qgs_expect_set;
  for (size_t i = 0; i < _qgs_expect.size(); ++i)
    qgs_expect_set.insert(_qgs_expect[i].gid);

  for (Json::Value::ArrayIndex i = 0; i < js["query_groups"].size(); ++i) {
    REQUIRE(js["query_groups"][i].isString());
    auto got_gid = js["query_groups"][i].asString();
    REQUIRE(qgs_expect_set.find(got_gid) != qgs_expect_set.end());
  }
}

void APIv1Tester::_testSetGetQG(const std::string &_token,
                                const QueryGroup &_query_group_expect) {

  httplib::Headers headers;
  headers.insert(std::make_pair("Authorization", "Bearer " + _token));

  httplib::Client cli{"http://" + s_test_addr + ":" +
                      std::to_string(s_test_port)};
  std::string body{_query_group_expect.asJson()};
  auto res = cli.Post("/set_query_group", headers, body.c_str(), body.size(),
                      APIv1::s_content_type_json);
  {
    // test status==ok
    Json::Value js;
    REQUIRE(Json::Reader().parse(res->body, js));
    REQUIRE(js["status"].isString());
    REQUIRE(js["status"].asString() == "ok");
  }
  // /get_query_group and compare
  {
    httplib::Client cli{"http://" + s_test_addr + ":" +
                        std::to_string(s_test_port)};
    auto res =
        cli.Get("/get_query_group?gid=" + _query_group_expect.gid, headers);
    Json::Value js;
    REQUIRE(Json::Reader().parse(res->body, js));
    REQUIRE(js["status"].isString());
    REQUIRE(js["status"].asString() == "ok");
    REQUIRE(!js["result"].isNull());
    auto query_group_got = QueryGroup::fromJson(js["result"]);
    REQUIRE(_query_group_expect == query_group_got);
  }
}

void APIv1Tester::_testSetGetQG(const std::vector<QueryGroup> &_qgs_expect) {

  std::string token = _testOAuth2GetToken();

  for (const auto &query_group_expect : _qgs_expect)
    _testSetGetQG(token, query_group_expect);
}

void APIv1Tester::_testHits(const std::vector<QueryGroup> &_qgs,
                            const std::vector<TestPage> &_pages) {
  std::string token = _testOAuth2GetToken();
  _testHitsImpl(token, _qgs, _pages);
}

void APIv1Tester::_testHitsImpl(const std::string &_token,
                                const std::vector<QueryGroup> &_qgs,
                                const std::vector<TestPage> &_pages) {
  //   fill pagedb
  //   wait for real query complete and check hits
  _fillPages(m_query_loop_state.page_db, _pages);

  // query thread in test
  std::thread th([&]() { m_query_loop_state.query_loop->loop(); });

  m_query_loop_state.query_loop->waitNextCycle();
  m_query_loop_state.query_loop->stop();
  th.join();

  auto query_groups_hits_expect_map = test_qgs_hits_map();

  for (auto query_group : _qgs) {
    auto hits_expect = query_groups_hits_expect_map[query_group.gid];

    _getHitsAndCompare(_token, query_group.gid, hits_expect);
  }
}

void APIv1Tester::_getHitsAndCompare(const std::string &_token,
                                     const GroupId &_gid,
                                     const QueryGroupHits &_hits_expect) {
  httplib::Headers headers;
  headers.insert(std::make_pair("Authorization", "Bearer " + _token));
  httplib::Client cli{"http://" + s_test_addr + ":" +
                      std::to_string(s_test_port)};
  auto res = cli.Get("/get_hits?gid=" + _gid, headers);

  std::cout << "/get_hits: " << res->body << std::endl;
  std::cout << "expect: " << _hits_expect.asJson() << std::endl;

  Json::Value js;
  REQUIRE(Json::Reader().parse(res->body, js));
  REQUIRE(js["status"].isString());
  REQUIRE(js["status"].asString() == "ok");
  REQUIRE(!js["result"].isNull());
  auto _hits_got = QueryGroupHits::fromJson(js["result"]);
  REQUIRE(_hits_expect == _hits_got);
}

APIv1Tester::APIv1Tester() {
#warning NOT_IMPLEMENTED
  // ORDER MATTERS!

  _setUp();
  // test1:
  //   setup api
  //   make simple call (version)
  _testSimpleCall();
  // test2
  _testOAuth2(); // /token, /check_token

  auto expect_qgs = test_qgs();
  // test3
  _testSetGetQG(expect_qgs);
  // test4
  _testListQG(expect_qgs);
  // test5
  //   delete QG
  // test6
  //   fill pagedb
  //   wait for real query complete and check hits
  auto pages = test_pages(); // consistent with test_qgs()
  _testHits(expect_qgs, pages);
}

} // namespace contentv1
