#pragma once
#include "aho_corasick.hpp"
#include "core/QueryGroup.h"
#include <unordered_map>
#include <vector>

namespace contentv1 {

struct QueryGroupMatcher {
  aho_corasick::trie m_trie;
  std::unordered_map<std::string, std::vector<GroupId>> m_keyword_groups;

  void _buildState(const std::vector<QueryGroup> &_qgs);

public:
  QueryGroupMatcher(const std::vector<QueryGroup> &_qgs);
  bool match(const std::string &_content, std::vector<GroupId> &_gids);
};

} // namespace contentv1
