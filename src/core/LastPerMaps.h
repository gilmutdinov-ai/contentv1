#pragma once

#include "core/UrlFreqMap.h"

namespace contentv1 {

class LastPerMaps {
  UrlFreqMap m_hour_map;
  UrlFreqMap m_day_map;

public:
  LastPerMaps();

  Cnt max() const;

  void insert(const UrlVisit &url_visit, Tp now);
  void insertVisitsPack(const UrlTpStat &_uts, Tp _now);

  void getTopHourUrls(std::vector<UrlFreq> &_ufv) const;
  void getTopDayUrls(std::vector<UrlFreq> &_ufv) const;

  void merge(const LastPerMaps &_a);
};

} // namespace contentv1
