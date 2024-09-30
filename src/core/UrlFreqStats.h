#pragma once

#include "core/LastPerMaps.h"

#include <functional>
#include <memory>

// accessed from LoadLoop & EnqueueLoop
// must be thread safe

namespace contentv1 {

class UrlFreqStats {
private:
  LastPerMaps m_maps;
  std::mutex m_mtx;

protected:
public:
  using Ptr = std::shared_ptr<UrlFreqStats>;

  virtual ~UrlFreqStats() = default;

  [[nodiscard]] virtual Cnt max();

  virtual void insert(const UrlVisit &url_visit, Tp now);
  virtual void insertVisitsPack(const UrlTpStat &_uts, Tp _now);

  // not guaranteed that limit, may return more (performance)
  virtual void getTopUrls(std::vector<UrlFreq> &_urls_v, int limit = -1);

  virtual void getTopUrls(std::vector<UrlFreq> &_r, int _limit,
                          std::function<bool(const Url &)> filter);
};
} // namespace contentv1
