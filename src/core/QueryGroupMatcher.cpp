#include "core/QueryGroupMatcher.h"

namespace contentv1 {

QueryGroupMatcher::QueryGroupMatcher(const std::vector<QueryGroup> &_qgs) {
  _buildState(_qgs);
}

void QueryGroupMatcher::_buildState(const std::vector<QueryGroup> &_qgs) {

  std::set<Query> query_set;
  for (const auto &qg : _qgs) {
    const auto &group_id = qg.gid;
    for (const auto &query : qg.queries) {
      m_keyword_groups[query].push_back(group_id);
      query_set.insert(query);
    }
  }
  for (const auto &query : query_set)
    m_trie.insert(query);
}

bool QueryGroupMatcher::match(const std::string &_content,
                              std::vector<GroupId> &_gids) {

  std::set<GroupId> gids_set;
  auto matches = m_trie.parse_text(_content); // emit type

  for (auto keyword_groups_end = m_keyword_groups.end();
       const auto &match : matches) {
    const auto query = match.get_keyword();
    auto it = m_keyword_groups.find(query);
    if (it != keyword_groups_end)
      for (const auto &gid_hit : it->second)
        gids_set.insert(gid_hit);
  }

  for (auto it : gids_set)
    _gids.push_back(it);
  if (_gids.empty())
    return false;
  return true;
}

} // namespace contentv1
