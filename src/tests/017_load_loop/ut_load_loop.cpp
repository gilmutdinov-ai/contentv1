#include <catch2/catch_test_macros.hpp>

#include "scheduler/default/ut/LoadLoopTester.h"
#include <filesystem>
#include <iostream>

using namespace contentv1;

TEST_CASE("Test LoadLoop", "[LoadLoop]") { LoadLoopTester tester; }
