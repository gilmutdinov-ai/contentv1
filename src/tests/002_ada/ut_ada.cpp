#include <catch2/catch_test_macros.hpp>

#include "ada.cpp"
#include "ada.h"

TEST_CASE("Test ada url parser", "[ada]") {

  auto url = ada::parse<ada::url>("https://www.ya.ru");
  REQUIRE(url);

  // url->set_protocol("http");
  REQUIRE(url->get_protocol() == "https:");
  REQUIRE(url->get_host() == "www.ya.ru");
}
