#include "worker/QueryLoop.h"
#include "core/QueryGroupMatcher.h"
#include "misc/Time.h"

namespace contentv1 {

QueryLoopConfig::QueryLoopConfig() { addInts({INT_QUERY_INTERVAL_SEC}); }

QueryLoop::QueryLoop(QueryDb::Ptr _query_db, IPageDb::Ptr _page_db,
                     const QueryLoopConfig &_config)
    : m_running{true}, m_query_db{_query_db}, m_page_db{_page_db},
      m_query_interval_sec{
          _config[QueryLoopConfig::INT_QUERY_INTERVAL_SEC].as<int>()} {}

void QueryLoop::stop() { m_running.store(false); }

void QueryLoop::loop() {
  while (m_running.load()) {
    auto start = misc::get_now();
    _loopImpl();
    auto end = misc::get_now();
    auto passed_secs =
        std::chrono::duration_cast<std::chrono::seconds>(end - start);
    auto interval_secs = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::seconds{m_query_interval_sec});
    if (interval_secs > passed_secs) {
      int to_sleep = interval_secs.count() - passed_secs.count();
      sleep(to_sleep);
    }
  }
  _loopImpl();
}

void QueryLoop::_loopImpl() {

  // get all QG from QueryDb
  std::vector<QueryGroup> qgs;
  std::vector<QueryGroupHits> qgs_hits;
  m_query_db->getAllQG(qgs);
  _query(qgs, qgs_hits);
  for (const auto &qg_hits : qgs_hits)
    m_query_db->saveQGHits(qg_hits);

  ++m_cycles_count;
}

void QueryLoop::_query(const std::vector<QueryGroup> &_qgs,
                       std::vector<QueryGroupHits> &_qgs_hits) {
  // build aho matcher
  QueryGroupMatcher matcher{_qgs};
  // query PageDb and write hits
  std::unordered_map<GroupId, QueryGroupHits> qgs_hits_map;
  m_page_db->scanFull([&](const Url &_url, const std::string &_content) {
    std::vector<GroupId> gids;
    matcher.match(_content, gids);
    for (auto gid : gids) {
      qgs_hits_map[gid].gid = gid;
      qgs_hits_map[gid].hits.push_back(_url);
    }
  });
  for (const auto &[gid, qg_hits] : qgs_hits_map)
    _qgs_hits.push_back(qg_hits);
}

void QueryLoop::waitNextCycle() {

  auto current = m_cycles_count.load();
  while (m_cycles_count.load() == current)
    sleep(1);
}

} // namespace contentv1