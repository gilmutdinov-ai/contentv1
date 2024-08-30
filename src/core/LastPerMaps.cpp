#include "LastPerMaps.h"

namespace contentv1 {

LastPerMaps::LastPerMaps()
    : m_hour_map{std::chrono::hours{1}}, m_day_map{std::chrono::days{1}} {}

Cnt LastPerMaps::max() const {
  return std::max(m_hour_map.size(), m_day_map.size());
}

void LastPerMaps::insert(const UrlVisit &url_visit, Tp now) {

  m_hour_map.insert(url_visit, now);
  m_day_map.insert(url_visit, now);
}

void LastPerMaps::insertVisitsPack(const UrlTpStat &_uts, Tp _now) {

  m_hour_map.insertVisitsPack(_uts, _now);
  m_day_map.insertVisitsPack(_uts, _now);
}

void LastPerMaps::getTopHourUrls(std::vector<UrlFreq> &_ufv) const {
  m_hour_map.getTopUrls(_ufv);
}

void LastPerMaps::getTopDayUrls(std::vector<UrlFreq> &_ufv) const {
  m_day_map.getTopUrls(_ufv);
}

void LastPerMaps::merge(const LastPerMaps &_a) {
  m_hour_map.merge(_a.m_hour_map);
  m_day_map.merge(_a.m_day_map);
}

} // namespace contentv1
