#include "UrlFreqStats.h"
#include "misc/Time.h"

namespace contentv1 {

Cnt UrlFreqStats::max() {
  std::lock_guard<std::mutex> m_lock(m_mtx);
  return m_maps.max();
}

void UrlFreqStats::insert(const UrlVisit &url_visit, Tp now) {

  std::lock_guard<std::mutex> m_lock(m_mtx);
  m_maps.insert(url_visit, now);
}

void UrlFreqStats::insertVisitsPack(const UrlTpStat &_uts, Tp _now) {
  std::lock_guard<std::mutex> m_lock(m_mtx);
  m_maps.insertVisitsPack(_uts, _now);
}

void UrlFreqStats::getTopUrls(std::vector<UrlFreq> &_urls_v, int limit) {
  getTopUrls(_urls_v, limit, [](const Url &) { return false; });
}

void UrlFreqStats::getTopUrls(std::vector<UrlFreq> &_r, int _limit,
                              std::function<bool(const Url &)> filter) {

  std::vector<UrlFreq> hour_sorted;
  std::vector<UrlFreq> day_sorted;
  {
    std::lock_guard<std::mutex> m_lock(m_mtx);
    m_maps.getTopHourUrls(hour_sorted);
    m_maps.getTopDayUrls(day_sorted);
  }

  std::unordered_map<Url, UrlFreq> r_map;
  for (size_t i = 0; i < hour_sorted.size(); ++i)
    if (!filter(hour_sorted[i].first) && r_map.size() < _limit)
      r_map[hour_sorted[i].first] = hour_sorted[i];

  for (size_t i = 0; i < day_sorted.size(); ++i)
    if (!filter(day_sorted[i].first) && r_map.size() < _limit) {
      if (r_map.find(day_sorted[i].first) == r_map.end())
        r_map[day_sorted[i].first] = day_sorted[i];
      else
        r_map[day_sorted[i].first].second += day_sorted[i].second;
    }

  _r.clear();
  auto end = r_map.end();
  for (auto it = r_map.begin(); it != end; ++it)
    _r.push_back(it->second);

  std::sort(_r.begin(), _r.end(), [](const UrlFreq &_a, const UrlFreq &_b) {
    return _a.second > _b.second;
  });
}

} // namespace contentv1
