#include "aho_corasick.hpp"
#include "core/ut/QueryGroupMatcherTester.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("Test aho_corasick", "[aho_corasick]") {

  std::set<std::string> ans{"he", "hers", "she"};

  aho_corasick::trie trie;
  trie.insert("hers");
  trie.insert("his");
  trie.insert("she");
  trie.insert("he");
  auto r = trie.parse_text("ushers"); // emit type
  // for (size_t i = 0; i < r.size(); ++i)

  REQUIRE(r.size() == ans.size());
  for (size_t i = 0; i < r.size(); ++i)
    REQUIRE(ans.find(r[i].get_keyword()) != ans.end());
}

TEST_CASE("Test QueryGroupMatcher", "[QueryGroupMatcher]") {
  contentv1::QueryGroupMatcherTester{};
}
