#include "api/v1/APIv1.h"
#include "misc/Base64.h"
#include "misc/Log.h"
#include "misc/Strings.h"
#include <iostream>

namespace contentv1 {

std::string APIv1::hash(const std::string &_str) {
#pragma message "REPLACE_WITH_REAL_CRYPTO_HASH"
  std::stringstream ss;
  ss << std::hash<std::string>{}(_str);
  return misc::base64_encode(ss.str());
}

bool APIv1::_auth(const httplib::Headers &_headers, std::string &_error_str) {
  // check Authorization: Bearer {token}
  std::string token;
  if (!_parseBearer(_headers, token, _error_str)) {
    LOG(_error_str);
    return false;
  }
  if (!_checkToken(token)) {
    _error_str = "bad token";
    LOG(_error_str);
    return false;
  }
  return true;
}

bool APIv1::_genToken(const std::string &_api_key,
                      const std::string &_api_secret_key, std::string &_token,
                      long &_expires_in) {

  // base64(_api_key)[:-5][6 random chars][base64(prev + pepper):-5]
  {
    std::lock_guard<std::mutex> lock(m_mtx);
    auto it = m_api_keys.find(_api_key);
    if (it == m_api_keys.end())
      return false;
    if (it->second != _api_secret_key)
      return false;
  }

  const std::string token_body =
      misc::last_n(misc::base64_encode(_api_key), 5) + misc::gen_random_str(6);
  const std::string token_hash = misc::last_n(hash(token_body + s_pepper), 5);

  _token = token_body + token_hash;
  _expires_in = s_token_expires_in;

  return true;
}

bool APIv1::_checkToken(const std::string &_token) {

  if (_token.size() != s_token_size)
    return false;

  const std::string token_body = _token.substr(0, 11);
  const std::string token_hash = misc::last_n(_token, 5);

  if (misc::last_n(hash(token_body + s_pepper), 5) != token_hash)
    return false;

  return true;
}

bool APIv1::_isToken(const std::string &_token) { return _checkToken(_token); }

} // namespace contentv1
