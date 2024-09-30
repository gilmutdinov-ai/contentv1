#include "UrlDb.h"
#include "misc/Time.h"

namespace contentv1 {

void UrlDb::setEnqueued(const std::vector<Url> &_in_urls, Tp now) {
  std::lock_guard<std::mutex> lock(m_mtx);
  for (size_t i = 0; i < _in_urls.size(); ++i)
    _setEnqueued(_in_urls[i], now);
}

void UrlDb::_setEnqueued(const Url &_url, Tp tp) {

  m_url_crawl_attempts[_url].add(tp);
}

void UrlDb::setResult(const Url &_url, CrawlAttemptsPb::AttemptStatus _status,
                      Tp now) {
  std::lock_guard<std::mutex> lock(m_mtx);
  auto it = m_url_crawl_attempts.find(_url);
  if (it == m_url_crawl_attempts.end()) {
    m_url_crawl_attempts[_url].add(now);
    it = m_url_crawl_attempts.find(_url);
  }
  auto &last_attempt = it->second.getLast();
  last_attempt.status = _status;
}

void UrlDb::onFetched(const std::vector<FetchResult> &_results) {
  const auto now = misc::get_now();

  std::lock_guard<std::mutex> lock(m_mtx);
  for (size_t i = 0; i < _results.size(); ++i)
    _setStatus(_results[i], now);
}

void UrlDb::_setStatus(const FetchResult &_res, Tp _now) {

  auto it = m_url_crawl_attempts.find(_res.gfq.url);
  if (it == m_url_crawl_attempts.end()) {
    m_url_crawl_attempts[_res.gfq.url].add(_now);
    it = m_url_crawl_attempts.find(_res.gfq.url);
  }
  auto &last_attempt = it->second.getLast();
  last_attempt.status = _res.status;
}
} // namespace contentv1
