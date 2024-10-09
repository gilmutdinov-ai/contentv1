#pragma once
#include "core/GFQRecord.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace contentv1 {

class ReqBatch {
  std::vector<GFQRecord> m_reqs;

public:
  void acc(const GFQRecord &_req);
  void merge(const ReqBatch &_a);

  [[nodiscard]] size_t size() const;
  [[nodiscard]] bool empty() const;
  size_t getReqs(size_t _max_urls_in_batch,
                 std::vector<GFQRecord> &_reqs) const;
  [[nodiscard]] ReqBatch slice(size_t _slice_count) const;

  void print() const;
};
} // namespace contentv1