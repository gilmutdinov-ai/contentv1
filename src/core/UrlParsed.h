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

  [[nodiscard]] bool ok() const;
  [[nodiscard]] Host host() const;
  [[nodiscard]] Url normalized() const;
  [[nodiscard]] Url robots() const;
  [[nodiscard]] bool isRobots() const;
  //  Url reversed() const;
};

} // namespace contentv1
