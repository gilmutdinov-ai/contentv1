#pragma once
#include "core/Types.h"

namespace contentv1 {

class UrlParsed {
  bool m_ok;
  Host m_host;
  Url m_normalized;
  Url m_robots;
  //  Url m_reversed;

public:
  UrlParsed(const Url &url);

  bool ok() const;
  Host host() const;
  Url normalized() const;
  Url robots() const;
  bool isRobots() const;
  //  Url reversed() const;
};

} // namespace contentv1
