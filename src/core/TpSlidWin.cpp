#include "TpSlidWin.h"

namespace contentv1 {

TpSlidWin::TpSlidWin(DurationSec _window) : m_window(_window) {}

Cnt TpSlidWin::size() { return size(std::chrono::system_clock::now()); }

Cnt TpSlidWin::size(Tp now) {
  checkClean(now);
  return static_cast<Cnt>(m_q.size());
}

void TpSlidWin::inc() { inc(std::chrono::system_clock::now()); }

void TpSlidWin::inc(Tp now) {
  m_q.push(now);
  checkClean(now);
}

void TpSlidWin::checkClean(Tp now) {
  while (!m_q.empty() && std::chrono::duration_cast<DurationSec>(
                             now - m_q.front()) > m_window) {
    m_q.pop();
  }
}

} // namespace contentv1
