#pragma once

#include "core/Types.h"
#include <chrono>
#include <queue>

namespace contentv1 {

/*
not thread safe,
just count events in last Duration
*/

class TpSlidWin {
  std::queue<Tp> m_q;
  DurationSec m_window;

  void checkClean(Tp now);

public:
  TpSlidWin(DurationSec _window);
  Cnt size();
  Cnt size(Tp now);
  void inc();
  void inc(Tp now);
};

} // namespace contentv1
