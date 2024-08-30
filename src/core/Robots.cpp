#include "Robots.h"
#include "robotstxt/robots.h"
#include <iostream>

namespace contentv1 {

Robots::Robots(const std::string &_robots_str) : m_robots_str(_robots_str) {}

bool Robots::isDisallowed(const Url &_url) const { return !isAllowed(_url); }

bool Robots::isAllowed(const Url &_url) const {

  if (m_robots_str.empty())
    return true;

  static const std::vector<std::string> s_match_user_agents{
      "Googlebot",
      "Mozilla/5.0 (compatible; YandexBot/3.0; +http://yandex.com/bots)"};

  // return ::googlebot::RobotsMatcher{}.AllowedByRobots(
  // m_robots_str, &s_match_user_agents, _url);
  ::googlebot::RobotsMatcher matcher;
  return matcher.OneAgentAllowedByRobots(m_robots_str, s_match_user_agents[1],
                                         _url);
}

} // namespace contentv1
