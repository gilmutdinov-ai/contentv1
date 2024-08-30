#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "core/UrlFreqStats.h"
#include "core/ut/UrlFreqStatsTester.h"

using namespace contentv1;

TEST_CASE("Test UrlFreqStats", "[UrlFreqStats]") {

  UrlFreqStatsTester<UrlFreqStats> tester;
}
