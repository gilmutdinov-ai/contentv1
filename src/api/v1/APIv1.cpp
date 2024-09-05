#include "api/v1/APIv1.h"
#include "misc/Base64.h"
#include "misc/Log.h"
#include "misc/Strings.h"
#include <iostream>

namespace contentv1 {

APIv1Config::APIv1Config() {
  addStrings({STR_LISTEN_ADDR});
  addInts({INT_LISTEN_PORT});
}

void APIv1::APIResponse::set(const std::string &_body, const std::string &_ct) {
  set(200, _body, _ct);
}

void APIv1::APIResponse::set(int _status, const std::string &_body,
                             const std::string &_ct) {
  status = _status;
  body = _body;
  content_type = _ct;
}

void APIv1::APIResponse::setAccessDenied(const std::string &_mess) {

  Json::Value js;
  js["status"] = "denied";
  js["message"] = _mess;
  set(httplib::StatusCode::Forbidden_403, Json::FastWriter{}.write(js),
      s_content_type_json);
}

void APIv1::APIResponse::setBadRequest(const std::string &_mess) {

  Json::Value js;
  js["status"] = "bad_request";
  js["message"] = _mess;
  set(httplib::StatusCode::BadRequest_400, Json::FastWriter{}.write(js),
      s_content_type_json);
}

void APIv1::APIResponse::setInternalError(const std::string &_mess) {
  Json::Value js;
  js["status"] = "internal_error";
  js["message"] = _mess;
  set(httplib::StatusCode::InternalServerError_500,
      Json::FastWriter{}.write(js), s_content_type_json);
}

APIv1::APIv1(QueryDb::Ptr _query_db, QueryLoop::Ptr _query_loop,
             const APIv1Config &_config)
    : m_query_loop(_query_loop), m_query_db(_query_db),
      m_listen_addr{_config[APIv1Config::STR_LISTEN_ADDR].asString()},
      m_listen_port{_config[APIv1Config::INT_LISTEN_PORT].asInt()},
      m_th{std::bind(&APIv1::_setupServer, this)} {
  // setup server in thread
  std::lock_guard<std::mutex> lock(m_mtx);

  // hardcoded keys
  assert(s_api_keys_hardcode.size() == 2);
  m_api_keys[std::string{s_api_keys_hardcode[0]}] =
      std::string{s_api_keys_hardcode[1]};
}

APIv1::~APIv1() {
  m_srv->stop();
  join();
}

void APIv1::join() {
  if (m_th.joinable())
    m_th.join();
}

void APIv1::_setupServer() {

  auto parseRequest = [](const httplib::Request &_req) {
    APIRequest api_req{_req.headers, _req.body, _req.params};
    return api_req;
  };

  auto sendResponse = [](const APIResponse &_api_res, httplib::Response &res) {
    res.status = _api_res.status;
    res.set_content(_api_res.body, _api_res.content_type);
  };

  auto sendInternalError = [](httplib::Response &res) {
    res.status = httplib::StatusCode::InternalServerError_500;
    res.set_content(s_internal_server_error_json, s_content_type_json);
  };

  ////// ROUTES NO AUTH
  m_routes.push_back({GET, "/version",
                      false, // need_auth
                      std::bind(&APIv1::hdlVersion, this, std::placeholders::_1,
                                std::placeholders::_2)});
  m_routes.push_back({GET, "/token",
                      false, // need_auth
                      std::bind(&APIv1::hdlToken, this, std::placeholders::_1,
                                std::placeholders::_2)});

  ////// ROUTES NEED AUTH
  m_routes.push_back({GET, "/check_token",
                      true, // need_auth
                      std::bind(&APIv1::hdlCheckToken, this,
                                std::placeholders::_1, std::placeholders::_2)});
  m_routes.push_back({GET, "/get_query_groups_list",
                      true, // need_auth
                      std::bind(&APIv1::hdlGetQueryGroupsList, this,
                                std::placeholders::_1, std::placeholders::_2)});
  m_routes.push_back({POST, "/set_query_group",
                      true, // need_auth
                      std::bind(&APIv1::hdlSetQueryGroup, this,
                                std::placeholders::_1, std::placeholders::_2)});
  m_routes.push_back({DELETE, "/delete_query_group",
                      true, // need_auth
                      std::bind(&APIv1::hdlDeleteQueryGroup, this,
                                std::placeholders::_1, std::placeholders::_2)});
  m_routes.push_back({GET, "/get_query_group",
                      true, // need_auth
                      std::bind(&APIv1::hdlGetQueryGroup, this,
                                std::placeholders::_1, std::placeholders::_2)});
  m_routes.push_back({GET, "/get_hits",
                      true, // need_auth
                      std::bind(&APIv1::hdlGetHits, this, std::placeholders::_1,
                                std::placeholders::_2)});
  ////// ROUTES END

  try {
    m_srv.reset(new httplib::Server);

    auto handler_wrap = [&](const Route &_route,
                            const httplib::Request &_httpreq,
                            httplib::Response &_httpres) {
      try {
        APIRequest api_req = parseRequest(_httpreq);
        APIResponse api_res;
        std::string error_str;
        if (_route.need_auth && !_auth(api_req.headers, error_str)) {
          api_res.setAccessDenied(error_str);
          sendResponse(api_res, _httpres);
          return; // lambda
        }
        _route.handler(api_req, api_res); // HANDLER
        sendResponse(api_res, _httpres);
      } catch (std::exception &_ex) {
        LOG("APIv1: " << _route.path << " ex:" << _ex.what());
        sendInternalError(_httpres);
      }
    };

    for (auto i = 0; i < m_routes.size(); ++i) {
      const auto &route = m_routes[i];
      if (route.method == GET) {
        m_srv->Get(route.path, [&](const httplib::Request &_httpreq,
                                   httplib::Response &_httpres) {
          handler_wrap(route, _httpreq, _httpres);
        });
      } else if (route.method == POST) {
        m_srv->Post(route.path, [&](const httplib::Request &_httpreq,
                                    httplib::Response &_httpres) {
          handler_wrap(route, _httpreq, _httpres);
        });
      } else if (route.method == DELETE) {
        m_srv->Delete(route.path, [&](const httplib::Request &_httpreq,
                                      httplib::Response &_httpres) {
          handler_wrap(route, _httpreq, _httpres);
        });
      }
    }
    m_srv->listen(m_listen_addr, m_listen_port);
  } catch (std::exception &_ex) {
    LOG(std::string("APIv1::_setupServer ex: ") << _ex.what());
  }
}

} // namespace contentv1
