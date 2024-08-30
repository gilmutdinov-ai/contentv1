#include <catch2/catch_test_macros.hpp>

#include "core/UrlParsed.h"

TEST_CASE("Test UrlParsed", "[UrlParsed]") {

  auto url_str{"https://www.ya.ru"};
  auto url_norm{"https://www.ya.ru/"};
  auto url_robots{"https://www.ya.ru/robots.txt"};
  auto host{"www.ya.ru"};

  auto url = contentv1::UrlParsed(url_str);
  REQUIRE(url.ok());
  REQUIRE(url.normalized() == url_norm);
  // std::cout << "resersed: " << url.reversed() << std::endl;
  REQUIRE(url.host() == host);
  REQUIRE(url.robots() == url_robots);
}
