#pragma once
#include "core/Types.h"
#include <vector>

namespace contentv1 {

class Robots {

  std::string m_robots_str;

public:
  Robots() = default;
  Robots(const std::string &_robots_str);
  Robots(const Robots &r) = default;
  Robots &operator=(const Robots &r) = default;
  bool isAllowed(const Url &url) const;
  bool isDisallowed(const Url &_url) const;
};

} // namespace contentv1
