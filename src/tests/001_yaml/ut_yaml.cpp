#include "yaml-cpp/yaml.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test yaml parser", "[yaml]") {

  const std::string config_path = "src/tests/001_yaml/ut_conf.yml";
  YAML::Node config = YAML::LoadFile(config_path);
  REQUIRE(config["apiv1_listen_port"].Type() == YAML::NodeType::Scalar);
  REQUIRE(config["apiv1_listen_port"].as<int>() == 6767);

  REQUIRE(config["page_db_scylla"].Type() == YAML::NodeType::Map);
  REQUIRE(config["page_db_scylla"]["scylla_hosts"].Type() ==
          YAML::NodeType::Sequence);
  REQUIRE(config["page_db_scylla"]["scylla_hosts"][0].Type() ==
          YAML::NodeType::Scalar);
  REQUIRE(config["page_db_scylla"]["scylla_hosts"][0].as<std::string>() ==
          "scylla-docker-scylla-contentv1-1");
  REQUIRE(config["_unexistent_key_"].Type() == YAML::NodeType::Undefined);
}
