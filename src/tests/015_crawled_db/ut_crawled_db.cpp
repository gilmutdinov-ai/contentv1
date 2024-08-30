#include <catch2/catch_test_macros.hpp>

#include "ldb/ut/CrawledDbTester.h"
#include <filesystem>
#include <iostream>

using namespace contentv1;

TEST_CASE("Test CrawledDb", "[CrawledDb]") { CrawledDbTester tester; }
