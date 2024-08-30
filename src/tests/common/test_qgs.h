#pragma once
#include "core/QueryGroup.h"
#include "tests/common/test_pages.h"
#include <map>

// Keep consistent with test pages in test_pages.h

namespace contentv1 {

std::vector<QueryGroup> test_qgs() {
  std::vector<QueryGroup> qgs;
  qgs.push_back(QueryGroup{"auto", {"vw", "lada"}});
  qgs.push_back(QueryGroup{"pets", {"cat", "dog"}});
  qgs.push_back(QueryGroup{"real", {"house", "apartement"}});
  return qgs;
}

std::map<GroupId, QueryGroupHits> test_qgs_hits_map() {
  auto qgs = test_qgs();
  auto pages = test_pages();
  auto pages_map = build_pages_map(pages);

  std::map<GroupId, QueryGroupHits> ret;
  for (auto page : pages) {

    for (auto gid : page.groups) {
      ret[gid].gid = gid;
      ret[gid].hits.push_back(page.url);
    }
  }
  for (auto &it : ret) {
    it.second.sortAndUniq();
  }
  return ret;
}

} // namespace contentv1