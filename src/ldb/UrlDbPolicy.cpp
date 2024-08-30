#include "UrlDb.h"

namespace contentv1 {

std::vector<EUrlCrawlDecisionPb>
UrlDb::leaveNeedUrls(const std::vector<UrlFreq> &_in_urls, Tp _now,
                     std::vector<UrlFreq> &_out_urls) {
  std::lock_guard<std::mutex> lock(m_mtx);
  std::vector<EUrlCrawlDecisionPb> decisions;
  for (size_t i = 0; i < _in_urls.size(); ++i) {
    auto decision = _IsNeedCrawl(_in_urls[i].first, _now);
    decisions.push_back(decision);
    if (decision == ALLOW)
      _out_urls.push_back(_in_urls[i]);
  }
  return decisions;
}

EUrlCrawlDecisionPb UrlDb::_IsNeedCrawl(const Url &_url, Tp _now) const {

  /*
      need_crawl? add to global queue
        never attempted - yes
        last attempt CURL_ERR - try in hour
        last attempt 4xx or 5xx - wait x2 each time
        last attempted enqueued - wait 1 day
        last attempt pending - wait 1 hour
        last attempt succeded - wait 1 week
  */

  auto it = m_url_crawl_attempts.find(_url);
  // never attempted - yes
  if (it == m_url_crawl_attempts.end() || it->second.empty())
    return ALLOW;

  const auto &attempts = it->second;
  const auto &last_attempt = attempts.getLast();
  // last attempt CURL_ERR - try in hour
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_CURL_ERR) {
    if (std::chrono::duration_cast<std::chrono::hours>(
            _now - last_attempt.tp) >= std::chrono::hours{1})
      return ALLOW;
    return DISALLOW_TRY_IN_HOUR;
  }
  // last attempt enqueued - wait 1 day
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_ENQUEUED) {
    if (std::chrono::duration_cast<std::chrono::hours>(
            _now - last_attempt.tp) >= std::chrono::hours{24})
      return ALLOW;
    return DISALLOW_TRY_IN_A_DAY;
  }
  // last attempt pending - wait 1 hour
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_PENDING) {
    if (std::chrono::duration_cast<std::chrono::hours>(
            _now - last_attempt.tp) >= std::chrono::hours{1})
      return ALLOW;
    return DISALLOW_TRY_IN_HOUR;
  }
  // last attempt succeded - wait 1 week
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_OK) {
    if (std::chrono::duration_cast<std::chrono::days>(_now - last_attempt.tp) >=
        std::chrono::days{7})
      return ALLOW;
    return DISALLOW_TRY_IN_A_WEEK;
  }
  // last attempt 4xx or 5xx - wait x2 each time
  if (last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_4xx ||
      last_attempt.status == CrawlAttemptsPb::ATTEMPT_STATUS_5xx) {

    size_t last_4xx5xx_count = attempts.last4xx5xxCount();
    int wait_hours = 1;
    for (size_t i = 0; i < last_4xx5xx_count; ++i)
      wait_hours *= 2;
    if (std::chrono::duration_cast<std::chrono::hours>(
            _now - last_attempt.tp) >= std::chrono::hours{wait_hours})
      return ALLOW;
    return DISALLOW_WAIT_X2_TIME;
  }
  // otherwise
  return DISALLOW_NO_REASON;
}

} // namespace contentv1
