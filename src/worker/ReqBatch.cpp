#include "ReqBatch.h"

namespace contentv1 {

void ReqBatch::acc(const GFQRecord &_req) { m_reqs.push_back(_req); }

void ReqBatch::merge(const ReqBatch &_a) {
  m_reqs.insert(m_reqs.end(), _a.m_reqs.begin(), _a.m_reqs.end());
}

size_t ReqBatch::size() const { return m_reqs.size(); }

bool ReqBatch::empty() const { return m_reqs.empty(); }

size_t ReqBatch::getReqs(size_t _max_urls_in_batch,
                         std::vector<GFQRecord> &_reqs) const {

  std::stringstream ss;
  size_t inserted_count = 0;
  for (size_t i = 0; i < std::min(_max_urls_in_batch, m_reqs.size()); ++i) {

    _reqs.push_back(m_reqs[i]);
    ++inserted_count;
  }
  return inserted_count;
}

ReqBatch ReqBatch::slice(size_t _slice_count) const {
  if (_slice_count == m_reqs.size())
    return ReqBatch{};
  if (_slice_count > m_reqs.size())
    throw std::invalid_argument("ReqBatch::slice _slice_count > size");

  ReqBatch ret;
  ret.m_reqs.reserve(m_reqs.size() - _slice_count);
  for (size_t i = _slice_count; i < m_reqs.size(); ++i)
    ret.m_reqs.push_back(m_reqs[i]);
  return ret;
}

void ReqBatch::print() const {

  std::cout << "ReqBatch: ";
  for (size_t i = 0; i < m_reqs.size(); ++i) {
    std::cout << m_reqs[i].url << " ";
  }
  std::cout << std::endl;
}

} // namespace contentv1