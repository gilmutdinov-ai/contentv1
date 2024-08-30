#include <catch2/catch_test_macros.hpp>

#include "scheduler/ut/SchedulerTester.h"
#include <filesystem>
#include <iostream>

using namespace contentv1;

TEST_CASE("Test Scheduler", "[Scheduler]") { SchedulerTester tester; }
