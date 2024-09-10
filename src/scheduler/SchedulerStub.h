#pragma once
#include "scheduler/IScheduler.h"
#include "src/proto/CrawlAttempts.pb.h"
#include <memory>
#include <vector>

namespace contentv1 {

class SchedulerStub : public IScheduler {
  std::vector<GFQRecord> m_crawled;
  std::vector<std::pair<std::string, std::string>> m_robots;
  std::vector<std::pair<GFQRecord, CrawlAttemptsPb::AttemptStatus>> m_failed;
  std::vector<std::pair<GFQRecord, CrawlAttemptsPb::AttemptStatus>>
      m_robots_failed;

public:
  virtual ~SchedulerStub() = default;

  void tryFetchUrls(const std::vector<GFQRecord> &_try_urls,
                    std::vector<GFQRecord> &_allowed_urls) override;

  virtual void onFetched(const std::vector<FetchResult> &_results) override;

private:
  void onSuccess(const FetchResult &_res);
  void onFail(const FetchResult &_res);
  void onRobots(const FetchResult &_res);
  void onRobotsFail(const FetchResult &_res);

  friend class FetchLoopTester;
};

}; // namespace contentv1
