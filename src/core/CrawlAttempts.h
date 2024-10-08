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

  CrawlAttempts(const std::string &_dump);
  CrawlAttempts() = default;
  [[nodiscard]] std::string dump();

  Tp add(Tp tp);
  [[nodiscard]] Tp add();
  [[nodiscard]] bool set(Tp tp, CrawlAttemptsPb::AttemptStatus _status);

  [[nodiscard]] size_t size() const;
  [[nodiscard]] bool empty() const;
  [[nodiscard]] const CrawlAttempt &getLast() const;
  [[nodiscard]] CrawlAttempt &getLast();
  [[nodiscard]] size_t last4xx5xxCount() const;

private:
  std::vector<CrawlAttempt> m_crawl_attempts; // methods expect sorted

  friend bool operator==(const CrawlAttempts &a, const CrawlAttempts &b);
};

bool operator==(const CrawlAttempts &a, const CrawlAttempts &b);
bool operator==(const CrawlAttempts::CrawlAttempt &a,
                const CrawlAttempts::CrawlAttempt &b);

} // namespace contentv1
