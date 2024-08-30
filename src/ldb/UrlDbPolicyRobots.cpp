#include "UrlDb.h"

namespace contentv1 {
std::vector<EUrlCrawlDecisionPb>
UrlDb::leaveNeedRobots(const std::vector<UrlFreq> &_in_robots, Tp _now,
                       std::vector<UrlFreq> &_out_robots) {
  std::lock_guard<std::mutex> lock(m_mtx);
  std::vector<EUrlCrawlDecisionPb> decisions;
  for (size_t i = 0; i < _in_robots.size(); ++i) {
    auto decision = _IsNeedCrawlRobots(_in_robots[i].first, _now);
    decisions.push_back(decision);
    if (decision == ALLOW)
      _out_robots.push_back(_in_robots[i]);
  }
  return decisions;
}

EUrlCrawlDecisionPb UrlDb::_IsNeedCrawlRobots(const Url &_robots_url,
                                              Tp _now) const {
  /*
      need_crawl? add to global queue
        never attempted - yes [OK]
        last attempt CURL_ERR - try in day [OK]
        last attempt 4xx or 5xx - wait 1 day [OK]
        last attempted enqueued - wait 1 hour [OK]
        last attempt pending - wait 1 hour [OK]
        last attempt succeded - wait 1 day [OK]
  */
  auto it = m_url_crawl_attempts.find(_robots_url);
  // never attempted - yes
  if (it == m_url_crawl_attempts.end() || it->second.empty())
    return ALLOW;

  const auto &attempts = it->second;
  const auto &last_attempt = attempts.getLast();
  // last attempt CURL_ERR - try in day
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_CURL_ERR) {
    if (std::chrono::duration_cast<std::chrono::hours>(
            _now - last_attempt.tp) >= std::chrono::hours{24})
      return ALLOW;
    return DISALLOW_TRY_IN_A_DAY;
  }
  // last attempt enqueued - wait 1 hour
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_ENQUEUED) {
    if (std::chrono::duration_cast<std::chrono::hours>(
            _now - last_attempt.tp) >= std::chrono::hours{1})
      return ALLOW;
    return DISALLOW_TRY_IN_HOUR;
  }
  // last attempt pending - wait 1 hour
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_PENDING) {
    if (std::chrono::duration_cast<std::chrono::hours>(
            _now - last_attempt.tp) >= std::chrono::hours{1})
      return ALLOW;
    return DISALLOW_TRY_IN_HOUR;
  }
  // last attempt succeded - wait 1 day
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_OK) {
    if (std::chrono::duration_cast<std::chrono::days>(_now - last_attempt.tp) >=
        std::chrono::hours{24})
      return ALLOW;
    return DISALLOW_TRY_IN_A_DAY;
  }
  // last attempt 4xx or 5xx - try in a day
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_4xx ||
      last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_5xx) {

    if (std::chrono::duration_cast<std::chrono::hours>(
            _now - last_attempt.tp) >= std::chrono::hours{24})
      return ALLOW;
    return DISALLOW_TRY_IN_A_DAY;
  }
  // otherwise
  return DISALLOW_NO_REASON;
}

} // namespace contentv1
