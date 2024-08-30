#pragma once
#include "core/Types.h"
#include <set>
#include <unordered_map>

// Keep consistent with queries in test_qgs.h

namespace contentv1 {

struct TestPage {
  Url url;
  std::string content;
  std::set<GroupId> groups;
};

std::unordered_map<Url, TestPage>
build_pages_map(const std::vector<TestPage> &pages) {
  std::unordered_map<Url, TestPage> ret;
  for (const auto &page : pages)
    ret[page.url] = page;
  return ret;
}

std::vector<TestPage> test_pages() {
  std::vector<TestPage> pages;
  {
    const std::string content{
        "adh iahsd jasd lada faytsfd nahbsdfatsfd asfdyta as"};
    const std::set<GroupId> expect_groups{"auto"};
    const Url expect_url{"https://auto.ru/"};
    pages.push_back(TestPage{expect_url, content, expect_groups});
  }
  {
    const std::string content{
        "adh iahsd jasd cat faytsfd nahbsdfatsfd asfdyta as"};
    const std::set<GroupId> expect_groups{"pets"};
    const Url expect_url{"https://cats.ru"};
    pages.push_back(TestPage{expect_url, content, expect_groups});
  }
  {
    const std::string content{
        "adh iahsd jasd cat faytsfd vw nahbsdfatsfd asfdyta as"};
    const std::set<GroupId> expect_groups{"auto", "pets"};
    const Url expect_url{"https://dzen.ru"};
    pages.push_back(TestPage{expect_url, content, expect_groups});
  }
  {
    const std::string content{"cathouse apartement adh iahsd jasd faytsfd vw "
                              "nahbsdfatsfd asfdyta as"};
    const std::set<GroupId> expect_groups{"auto", "pets", "real"};
    const Url expect_url{"https://dzen.ru"};
    pages.push_back(TestPage{expect_url, content, expect_groups});
  }
  {
    const std::string content{"adh iahsd jasd faytsfd nahbsdfatsfd asfdyta as"};
    const std::set<GroupId> expect_groups{};
    const Url expect_url{"https://example.com"};
    pages.push_back(TestPage{expect_url, content, expect_groups});
  }
  return pages;
}
} // namespace contentv1