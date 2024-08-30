#include <catch2/catch_test_macros.hpp>

#include "core/Robots.h"
#include "tests/common/robots_str.h"

using namespace contentv1;

TEST_CASE("Test Robots", "[Robots]") {
  Robots r{g_robots_test_str};
  Url url_allow{"https://www.kp.ru"};
  Url url_disallow{"https://www.kp.ru/banners/x/"};
  REQUIRE(r.isAllowed(url_allow));
  REQUIRE(r.isDisallowed(url_disallow));
}
