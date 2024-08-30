#pragma once

#include "core/Types.h"
#include "src/proto/CrawlAttempts.pb.h"

/*
CrawlAttempt::AttemptStatus
  - отслеживает жизненный путь попытки скачать от решения положить в очередь
(CrawlAttempt::tp) до результата фетча
  - фетч - конкретно дернуть урл в воркере


  //   ATTEMPT_STATUS_ENQUEUED = 0, -  не декрементится когда PENDING
  //   ATTEMPT_STATUS_PENDING = 1,
*/

namespace contentv1 {
class CrawlAttempts {
  static constexpr size_t LAST_STORE_ATTEMPTS = 10;

public:
  struct CrawlAttempt {
    Tp tp; // put to global queue
    CrawlAttemptsPb::AttemptStatus status;
  };

  Tp add();
  Tp add(Tp tp);
  bool set(Tp tp, CrawlAttemptsPb::AttemptStatus _status);

  size_t size() const;
  bool empty() const;
  const CrawlAttempt &getLast() const;
  CrawlAttempt &getLast();
  size_t last4xx5xxCount() const;

  CrawlAttempts(const std::string &_dump);
  CrawlAttempts() = default;
  std::string dump();

private:
  std::vector<CrawlAttempt> m_crawl_attempts; // methods expect sorted

  friend bool operator==(const CrawlAttempts &a, const CrawlAttempts &b);
};

bool operator==(const CrawlAttempts &a, const CrawlAttempts &b);
bool operator==(const CrawlAttempts::CrawlAttempt &a,
                const CrawlAttempts::CrawlAttempt &b);

} // namespace contentv1
