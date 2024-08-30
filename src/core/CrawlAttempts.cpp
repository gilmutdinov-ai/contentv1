#include "CrawlAttempts.h"
#include "misc/Time.h"

/*

https://stackoverflow.com/questions/31255486/how-do-i-convert-a-stdchronotime-point-to-long-and-back

*/

namespace contentv1 {

std::string CrawlAttempts::dump() {
  CrawlAttemptsPb pb;
  for (size_t i = 0; i < m_crawl_attempts.size(); ++i) {
    auto attempt_pb = pb.add_crawl_attempts();
    auto ts = misc::to_int64(m_crawl_attempts[i].tp);
    attempt_pb->set_ts(ts);
    attempt_pb->set_status(m_crawl_attempts[i].status);
  }
  std::string ret;
  pb.SerializeToString(&ret);
  return ret;
}

CrawlAttempts::CrawlAttempts(const std::string &_dump) {
  CrawlAttemptsPb pb;
  if (!pb.ParseFromString(_dump))
    throw std::invalid_argument("CrawlAttempts::CrawlAttempts parsing");
  for (size_t i = 0; i < pb.crawl_attempts_size(); ++i) {
    auto attempt_pb = pb.crawl_attempts(i);
    auto tp = misc::from_int64(attempt_pb.ts());
    m_crawl_attempts.push_back(CrawlAttempt{tp, attempt_pb.status()});
  }
}

Tp CrawlAttempts::add() { return add(std::chrono::system_clock::now()); }

Tp CrawlAttempts::add(Tp tp) {

  auto tp_r = misc::round_secs(tp);

  m_crawl_attempts.push_back(
      CrawlAttempt{tp_r, CrawlAttemptsPb::ATTEMPT_STATUS_ENQUEUED});
  while (m_crawl_attempts.size() > LAST_STORE_ATTEMPTS)
    m_crawl_attempts.erase(m_crawl_attempts.begin());
  return tp;
}

bool CrawlAttempts::set(Tp tp, CrawlAttemptsPb::AttemptStatus _status) {
  for (size_t i = 0; i < m_crawl_attempts.size(); ++i)
    if (m_crawl_attempts[i].tp == tp) {
      m_crawl_attempts[i].status = _status;
      return true;
    }
  return false;
}

size_t CrawlAttempts::size() const { return m_crawl_attempts.size(); }

bool CrawlAttempts::empty() const { return size() == 0; }

const CrawlAttempts::CrawlAttempt &CrawlAttempts::getLast() const {
  if (m_crawl_attempts.empty())
    throw std::runtime_error("CrawlAttempts::getLast on empty");
  return m_crawl_attempts[m_crawl_attempts.size() - 1];
}

CrawlAttempts::CrawlAttempt &CrawlAttempts::getLast() {
  if (m_crawl_attempts.empty())
    throw std::runtime_error("CrawlAttempts::getLast on empty");
  return m_crawl_attempts[m_crawl_attempts.size() - 1];
}

size_t CrawlAttempts::last4xx5xxCount() const {
  if (empty())
    return 0;
  size_t r = 0;
  size_t i = m_crawl_attempts.size() - 1;
  while (i > 0) {
    if (m_crawl_attempts[i].status != CrawlAttemptsPb::ATTEMPT_STATUS_4xx &&
        m_crawl_attempts[i].status != CrawlAttemptsPb::ATTEMPT_STATUS_5xx)
      break;
    ++r;
    --i;
  }
  return r;
}

bool operator==(const CrawlAttempts &a, const CrawlAttempts &b) {
  if (a.m_crawl_attempts.size() != b.m_crawl_attempts.size())
    return false;
  for (size_t i = 0; i < a.m_crawl_attempts.size(); ++i)
    if (a.m_crawl_attempts[i] != b.m_crawl_attempts[i])
      return false;
  return true;
}

bool operator==(const CrawlAttempts::CrawlAttempt &a,
                const CrawlAttempts::CrawlAttempt &b) {
  // int64_t ts1 = a.tp.time_since_epoch().count();
  // int64_t ts2 = b.tp.time_since_epoch().count();

  // std::cout << "ts1: " << ts1 << std::endl;
  // std::cout << "ts2: " << ts2 << std::endl;

  return a.tp == b.tp && a.status == b.status;
}

} // namespace contentv1
