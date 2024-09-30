#include "api/v1/APIv1.h"
#include "misc/Base64.h"
#include "misc/Strings.h"
#include <iostream>

namespace contentv1 {

bool APIv1::_parseApiKeys(const httplib::Headers &_headers,
                          std::string &_api_key, std::string &_api_secret_key,
                          std::string &_error_str) {
  auto it = _headers.find(s_auth_basic_header);
  if (it == _headers.end()) {
    _error_str = s_auth_basic_header + " header not found";
    return false;
  }
  auto auth_header = it->second;
  if (auth_header.size() < s_header_basic_min_size) {
    _error_str = s_auth_basic_header + " header too small";
    return false;
  }
  const auto auth_info =
      misc::base64_decode(auth_header.substr(s_header_basic_prefix.size()));
  const auto auth_tokens = misc::split(auth_info, ':');
  if (auth_tokens.size() != 2 || auth_tokens[0].empty() ||
      auth_tokens[1].empty()) {
    _error_str = s_auth_basic_header +
                 " header parsing error, expecting: 'Basic "
                 "$API_KEY:$API_SECRET_KEY'";
    return false;
  }

  _api_key = auth_tokens[0];
  _api_secret_key = auth_tokens[1];
  return true;
}

bool APIv1::_parseBearer(const httplib::Headers &_headers, std::string &_token,
                         std::string &_error_str) {

  auto it = _headers.find(s_auth_basic_header);
  if (it == _headers.end()) {
    _error_str = s_auth_basic_header + " header not found";
    return false;
  }
  const auto auth_header = it->second;
  if (auth_header.size() != s_header_bearer_exact_size) {
    _error_str = s_auth_basic_header + " header wrong size";
    return false;
  }
  _token = auth_header.substr(s_header_bearer_prefix.size());
  return true;
}

} // namespace contentv1
