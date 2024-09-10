#include "scheduler/default/ut/EnqueueLoopTester.h"
#include <catch2/catch_test_macros.hpp>

using namespace contentv1;

std::unique_ptr<EnqueueLoopTester> el_tester;

TEST_CASE("Test EnqueueLoop", "[EnqueueLoop]") { EnqueueLoopTester tester; }
