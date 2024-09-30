#include "UrlFreqMap.h"
#include "misc/Time.h"
#include <chrono>
#include <iostream>

namespace contentv1 {

UrlFreqMap::UrlFreqMap(DurationSec _window) : m_window{_window} {};

void UrlFreqMap::insert(const UrlVisit &url_visit, Tp _now) {
  UrlTpStat uts{url_visit};
  m_uts_q.push_back(uts);
  m_url_freq[url_visit.url] += uts.cnt;

  checkCleanup(_now);
}

void UrlFreqMap::insertVisitsPack(const UrlTpStat &_uts, Tp _now) {
  m_uts_q.push_back(_uts);
  m_url_freq[_uts.url] += _uts.cnt;

  checkCleanup(_now);
}

void UrlFreqMap::merge(const UrlFreqMap &_a) {

  // merge maps
  auto end = _a.m_url_freq.end();
  for (auto it = _a.m_url_freq.begin(); it != end; ++it)
    m_url_freq[it->first] += it->second;

  // merge 2 sorted queues to 1 sorted and write to m_uts_q
  std::deque<UrlTpStat> rq;

  auto it1 = m_uts_q.begin();
  auto end1 = m_uts_q.end();
  auto it2 = _a.m_uts_q.begin();
  auto end2 = _a.m_uts_q.end();

  while (it1 != end1 || it2 != end2) {
    if (it1 != end1 && it2 != end2) {
      if (it1->tp < it2->tp) {
        rq.push_back(*it1);
        ++it1;
      } else {
        rq.push_back(*it2);
        ++it2;
      }
    } else if (it1 != end1) {
      while (it1 != end1) {
        rq.push_back(*it1);
        ++it1;
      }
    } else { // it2 != end2
      while (it2 != end2) {
        rq.push_back(*it2);
        ++it2;
      }
    }
  }
  m_uts_q = std::move(rq);
}

void UrlFreqMap::checkCleanup(Tp _now) {

  if (m_uts_q.empty())
    return;
  /*
  std::cout << "UrlFreqMap::checkCleanup now:" << to_readable(_now)
            << std::endl;
  std::cout << "UrlFreqMap::checkCleanup fro:"
            << to_readable(m_uts_q.front().tp) << std::endl;
  std::cout << "UrlFreqMap::checkCleanup bak:" << to_readable(m_uts_q.back().tp)
            << std::endl;
  std::cout << "UrlFreqMap::checkCleanup win:" << to_readable_dur(m_window)
            << std::endl;

  auto diff =
      std::chrono::duration_cast<DurationSec>(_now - m_uts_q.front().tp);

  std::cout << "UrlFreqMap::checkCleanup dif:" << to_readable_dur(diff)
            << std::endl;
  std::cout << std::endl;
*/
  while (!m_uts_q.empty()) {

    auto diff =
        std::chrono::duration_cast<DurationSec>(_now - m_uts_q.front().tp);
    if (diff <= m_window)
      break;

    // std::cout << "UrlFreqMap::checkCleanup popping" << std::endl;
    auto uts = m_uts_q.front();
    auto it = m_url_freq.find(uts.url);
    if (it != m_url_freq.end()) {
      it->second -= uts.cnt;
      if (it->second <= 0)
        m_url_freq.erase(it);
    }
    m_uts_q.pop_front();
  }
}

bool UrlFreqMap::checkSortedAndDestroyQueue() {

  if (m_uts_q.empty())
    return true;

  auto prev = m_uts_q.front();
  m_uts_q.pop_front();

  while (!m_uts_q.empty()) {

    auto cur = m_uts_q.front();
    m_uts_q.pop_front();
    if (prev.tp > cur.tp)
      return false;
  }
  return true;
}

void UrlFreqMap::getTopUrls(std::vector<UrlFreq> &_urls_v) const {
  _urls_v.clear();
  auto end = m_url_freq.end();
  for (auto it = m_url_freq.begin(); it != end; ++it) {
    _urls_v.push_back(UrlFreq{it->first, it->second});
  }
  std::sort(_urls_v.begin(), _urls_v.end(),
            [](const auto &a, const auto &b) { return a.second > b.second; });
}

size_t UrlFreqMap::size() const { return m_url_freq.size(); }

} // namespace contentv1
