#pragma once

#include <sstream>
#include <string>

namespace contentv1 {

// normalized
static const std::string g_test_url_base{"https://www.kp.ru/articles/"}; // {i}/

std::string gen_test_url(int _id) {
  std::stringstream ss;
  ss << g_test_url_base << _id << "/";
  return ss.str();
}

int parse_test_url_id(const Url &_url) {

  size_t e = _url.size() - 1;
  size_t b = e - 1;
  while (b > 0 && _url[b - 1] != '/')
    --b;

  if (b == 0)
    throw std::invalid_argument("parse_test_url_id " + _url);

  return stoi(_url.substr(b, b - e));
}

} // namespace contentv1
