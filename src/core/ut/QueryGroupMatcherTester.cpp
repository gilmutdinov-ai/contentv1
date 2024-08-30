#include "core/ut/QueryGroupMatcherTester.h"
#include <iostream>

namespace contentv1 {
QueryGroupMatcherTester::QueryGroupMatcherTester() {

  std::vector<QueryGroup> qgs;
  qgs.push_back(QueryGroup{"auto", {"vw", "lada"}});
  qgs.push_back(QueryGroup{"pets", {"cat", "dog"}});
  qgs.push_back(QueryGroup{"real", {"house", "apartement"}});
  QueryGroupMatcher m(qgs);
  // bool match(const std::string &_content, std::vector<GroupId> &_gids);

  auto testContent = [&](const std::string &_content,
                         const std::vector<GroupId> &_expect_groups,
                         bool _print = false) {
    std::vector<GroupId> gids;
    bool found = m.match(_content, gids);
    if (_expect_groups.empty()) {
      REQUIRE(!found);
      REQUIRE(gids.empty());
      return;
    }
    REQUIRE(found);
    REQUIRE(!gids.empty());
    std::set<GroupId> gids_set{gids.begin(), gids.end()};
    REQUIRE(gids_set.size() == _expect_groups.size());

    for (auto ex : _expect_groups)
      REQUIRE(gids_set.find(ex) != gids_set.end());

    if (_print) {
      std::cout << "matched g:" << std::endl;
      for (auto g : gids)
        std::cout << g << std::endl;
    }
  };
  {
    const std::string content{
        "adh iahsd jasd lada faytsfd nahbsdfatsfd asfdyta as"};
    const std::vector<GroupId> expect_groups{"auto"};
    testContent(content, expect_groups);
  }
  {
    const std::string content{
        "adh iahsd jasd cat faytsfd nahbsdfatsfd asfdyta as"};
    const std::vector<GroupId> expect_groups{"pets"};
    testContent(content, expect_groups);
  }
  {
    const std::string content{
        "adh iahsd jasd cat faytsfd vw nahbsdfatsfd asfdyta as"};
    const std::vector<GroupId> expect_groups{"auto", "pets"};
    testContent(content, expect_groups);
  }
  {
    const std::string content{"cathouse apartement adh iahsd jasd faytsfd vw "
                              "nahbsdfatsfd asfdyta as"};
    const std::vector<GroupId> expect_groups{"auto", "pets", "real"};
    testContent(content, expect_groups);
  }
  {
    const std::string content{"adh iahsd jasd faytsfd nahbsdfatsfd asfdyta as"};
    const std::vector<GroupId> expect_groups{};
    testContent(content, expect_groups);
  }
}
} // namespace contentv1