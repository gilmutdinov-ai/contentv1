#include <catch2/catch_test_macros.hpp>

#include "scheduler/default/ut/SchedulerServerTester.h"
#include <filesystem>
#include <iostream>

using namespace contentv1;

TEST_CASE("Test Scheduler Server", "[SchedulerServer]") {
  SchedulerServerTester{};
}
