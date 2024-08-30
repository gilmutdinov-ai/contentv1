#include "SchedulerStub.h"
#include "core/UrlParsed.h"
#include "misc/Bazel.h"
#include <cstdlib>
#include <set>

namespace contentv1 {

void SchedulerStub::tryFetchUrls(const std::vector<GFQRecord> &_try_urls,
                                 std::vector<GFQRecord> &_allowed_urls) {
  for (size_t i = 0; i < _try_urls.size(); ++i) {

    UrlParsed url{_try_urls[i].url};
    if (!url.ok())
      continue;

    if (url.isRobots()) { // for test
      _allowed_urls.push_back(_try_urls[i]);
      continue;
    }

    int act = std::rand() % 2;
    switch (act) {
    case 0:
      _allowed_urls.push_back(_try_urls[i]);
      break;
    case 1:
      // disallowed
      break;
    }
  }
}

void SchedulerStub::onFetched(const std::vector<FetchResult> &_results) {
  for (size_t i = 0; i < _results.size(); ++i) {
    if (_results[i].isRobots()) {
      if (_results[i].isOk())
        onRobots(_results[i]);
      else
        onRobotsFail(_results[i]);
    } else { // regular page
      if (_results[i].isOk())
        onSuccess(_results[i]);
      else
        onFail(_results[i]);
    }
  }
}

void SchedulerStub::onSuccess(const FetchResult &_res) {
  m_crawled.push_back(_res.gfq);
}

void SchedulerStub::onFail(const FetchResult &_res) {
  m_failed.push_back(std::make_pair(_res.gfq, _res.status));
}

void SchedulerStub::onRobots(const FetchResult &_res) {
  m_robots.push_back(std::make_pair(_res.gfq.url, _res.content));
}

void SchedulerStub::onRobotsFail(const FetchResult &_res) {
  m_robots_failed.push_back(std::make_pair(_res.gfq, _res.status));
}

} // namespace contentv1
