#pragma once

#include "core/UrlFreqStats.h"
#include "misc/MergeQueue.h"
#include "misc/Time.h"
#include <functional>
#include <memory>

// accessed from LoadLoop & EnqueueLoop
// optimized for fast reading & fast writing (MergeQueue)
//
namespace contentv1 {

using UFSMergeQueue = misc::MergeQueue<UrlTpStat, LastPerMaps>;

class UrlFreqStatsLF : public UFSMergeQueue, public UrlFreqStats {

  std::unique_ptr<LastPerMaps> m_maps;

public:
  using Ptr = std::shared_ptr<UrlFreqStatsLF>;

  UrlFreqStatsLF(int _merge_period_secs = 2);
  virtual ~UrlFreqStatsLF();

  // UrlFreqStats override

  [[nodiscard]] Cnt max() override;

  void insert(const UrlVisit &url_visit, Tp now) override;
  void insertVisitsPack(const UrlTpStat &_uts, Tp _now) override;

  void getTopUrls(std::vector<UrlFreq> &_urls_v, int limit = -1) override;

  void getTopUrls(std::vector<UrlFreq> &_r, int _limit,
                  std::function<bool(const Url &)> filter) override;

  // MergeQueue

  virtual void accEvent(LastPerMaps &_ufms, const UrlTpStat &_ev) override {
    _ufms.insertVisitsPack(_ev, misc::get_now());
  }
  virtual LastPerMaps *copyGlobalObj() override {
    return new LastPerMaps{*m_maps};
  }
  virtual void merge(LastPerMaps &_a, const LastPerMaps &_b) override {
    _a.merge(_b);
  }
  virtual void switchGlobalObj(LastPerMaps *_a) override { m_maps.reset(_a); }
};
} // namespace contentv1
