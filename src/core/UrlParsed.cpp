#include "core/UrlParsed.h"
#include "ada.cpp"
#include "ada.h"
#include <sstream>

namespace contentv1 {

UrlParsed::UrlParsed(const Url &_url) {
  auto url = ada::parse<ada::url>(_url);
  m_ok = true;
  if (!url) {
    m_ok = false;
    return;
  }
  m_host = url->get_host();
  m_normalized = url->get_href();

  std::stringstream ss;
  ss << url->get_protocol() << "//" << m_host << "/robots.txt";
  m_robots = ss.str();
  // url->get_query();
  // std::cout << url->get_protocol() << std::endl;
  // std::cout << url->get_host() << std::endl;
}

bool UrlParsed::ok() const { return m_ok; }

Host UrlParsed::host() const { return m_host; }

Url UrlParsed::normalized() const { return m_normalized; }

Url UrlParsed::robots() const { return m_robots; }

bool UrlParsed::isRobots() const {
  return m_normalized.ends_with("/robots.txt");
}

// Url UrlParsed::reversed() const { return m_reversed; }

} // namespace contentv1
