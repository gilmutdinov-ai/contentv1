#pragma once

#include "core/Types.h"
#include "core/UrlTpStat.h"
#include "core/UrlVisit.h"
#include <deque>
#include <string>
#include <unordered_map>

namespace contentv1 {

// keep freq, remove old, get top
// sliding window, keep queue
// not thread safe, non persistent
// expects right UrlVisit.ts order

class UrlFreqMap {
public:
  UrlFreqMap() = delete;
  UrlFreqMap(DurationSec _dur);

  // WRITE
  void insert(const UrlVisit &url_visit, Tp now);
  void insertVisitsPack(const UrlTpStat &_uts, Tp _now);
  void merge(const UrlFreqMap &_a);
  // READ
  void getTopUrls(std::vector<UrlFreq> &_urls_v) const;
  size_t size() const;

  bool checkSortedAndDestroyQueue();

private:
  void checkCleanup(Tp now);

private:
  std::unordered_map<Url, long> m_url_freq;
  std::deque<UrlTpStat> m_uts_q; // for const iteration

  DurationSec m_window;
};
} // namespace contentv1
