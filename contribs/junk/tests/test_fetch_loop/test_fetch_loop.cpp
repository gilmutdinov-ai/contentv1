#include <catch2/catch_test_macros.hpp>

#include "src/worker/ut/FetchLoopTester.h"

TEST_CASE("Test FetchLoop", "[FetchLoop]") {
  contentv1::FetchLoopTester{false}; // no bazel test
}