#include <catch2/catch_test_macros.hpp>

#include "src/bin/contentd/cli/ut/ContentdTester.h"

TEST_CASE("Test Contentd", "[Contentd]") { contentv1::ContentdTester{}; }
