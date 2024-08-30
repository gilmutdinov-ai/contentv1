#include "api/v1/APIv1.h"
#include "misc/Log.h"

namespace contentv1 {

void APIv1::hdlGetHits(const APIRequest &_api_req, APIResponse &_api_res) {
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
  QueryGroupHits hits;
  if (!m_query_db->loadQGHits(gid, hits)) {
    // empty, may not queried yet
  }
  {
    Json::Value js;
    js["status"] = "ok";
    js["result"] = hits.toJsonValue();
    _api_res.set(Json::FastWriter{}.write(js));
  }
}
} // namespace contentv1
