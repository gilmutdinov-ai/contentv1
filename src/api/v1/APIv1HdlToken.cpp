#include "api/v1/APIv1.h"

namespace contentv1 {

template <typename T> static void print_mm(const T &mm) {
  for (const auto &[k, v] : mm) {
    std::cout << k << "|" << v << std::endl;
  }
  std::cout << std::endl;
}

void APIv1::hdlToken(const APIRequest &_api_req, APIResponse &_api_res) {

  const auto &_headers = _api_req.headers;

  // std::cout << "/token headers: \n";
  // print_mm(_headers);

  std::string api_key;
  std::string api_secret_key;
  std::string error_str;
  if (!_parseApiKeys(_headers, api_key, api_secret_key, error_str)) {
    _api_res.setAccessDenied(error_str);
    return;
  }

  // std::cout << "api_key: " << api_key << std::endl;
  // std::cout << "api_secret_key: " << api_secret_key << std::endl;
  //  call methods
  {
    std::string token;
    long expires_in;
    if (!_genToken(api_key, api_secret_key, token, expires_in)) {
      _api_res.setAccessDenied("check API keys");
      return;
    }
    Json::Value js;
    js["status"] = "ok";
    js["access_token"] = token;
    js["token_type"] = "Bearer";
    js["expires_in"] = expires_in;
    _api_res.set(Json::FastWriter{}.write(js));
  }
}

void APIv1::hdlCheckToken(const APIRequest &_api_req, APIResponse &_api_res) {

  Json::Value js;
  js["status"] = "ok";
  _api_res.set(Json::FastWriter{}.write(js));
}

} // namespace contentv1
