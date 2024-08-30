#include "api/v1/APIv1.h"
#include "misc/Log.h"

namespace contentv1 {

void APIv1::hdlGetQueryGroupsList(const APIRequest &_api_req,
                                  APIResponse &_api_res) {
  std::vector<QueryGroup> cgs;
  m_query_db->getAllQG(cgs);

  Json::Value js;
  Json::Value query_groups_arr_js;
  for (const auto &cg : cgs) {

    query_groups_arr_js.append(std::string(cg.gid));
  }
  js["status"] = "ok";
  js["query_groups_count"] = cgs.size();
  if (!cgs.empty())
    js["query_groups"] = query_groups_arr_js;
  _api_res.set(Json::FastWriter{}.write(js));
}

void APIv1::hdlSetQueryGroup(const APIRequest &_api_req,
                             APIResponse &_api_res) {
  Json::Value js;
  if (!Json::Reader().parse(_api_req.body, js)) {
    static std::string err{"err: parsing json req body"};
    LOG(err);
    _api_res.setBadRequest(err);
    return;
  }
  auto qg = QueryGroup::fromJson(_api_req.body);
  std::string err_str;
  if (!m_query_db->saveQG(qg, err_str)) {
    std::string err{"err: set gid:" + qg.gid};
    LOG(err);
    _api_res.setInternalError(err);
    return;
  }
  {
    Json::Value js;
    js["status"] = "ok";
    _api_res.set(Json::FastWriter{}.write(js));
  }
}

void APIv1::hdlGetQueryGroup(const APIRequest &_api_req,
                             APIResponse &_api_res) {
  GroupId gid;
  {
    auto it = _api_req.params.find("gid");
    if (it == _api_req.params.end()) {
      static std::string err{"err: parsing gid param"};
      LOG(err);
      _api_res.setBadRequest(err);
      return;
    }
    gid = it->second;
  }
  QueryGroup qg;
  if (!m_query_db->loadQG(gid, qg)) {
    static std::string err{"err: not exist"};
    LOG(err);
    _api_res.setBadRequest(err);
    return;
  }
  {
    Json::Value js;
    js["status"] = "ok";
    js["result"] = qg.toJsonValue();
    _api_res.set(Json::FastWriter{}.write(js));
  }
}

void APIv1::hdlDeleteQueryGroup(const APIRequest &_api_req,
                                APIResponse &_api_res) {
  GroupId gid;
  {
    auto it = _api_req.params.find("gid");
    if (it == _api_req.params.end()) {
      static std::string err{"err: parsing gid param"};
      LOG(err);
      _api_res.setBadRequest(err);
      return;
    }
    gid = it->second;
  }
  // Delete group and hits
  std::string err_str;
  std::string err_hits_str;
  if (!m_query_db->deleteQG(gid, err_str) ||
      !m_query_db->deleteQGHits(gid, err_hits_str)) {
    std::string err{"err: deleting gid:" + gid};
    LOG("deleteQG:" << err_str << " deleteQGHits:" << err_hits_str);
    _api_res.setBadRequest(err);
    return;
  }
  Json::Value js;
  js["status"] = "ok";
  _api_res.set(Json::FastWriter{}.write(js));
}

} // namespace contentv1
