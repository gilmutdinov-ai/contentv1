#include "SchedulerTester.h"
#include "misc/Bazel.h"
#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace contentv1 {

SchedulerTester::SchedulerTester() {

  std::filesystem::remove_all("/tmp/sched_ut");

  auto config_path{misc::get_bazel_test_data_path(s_conf_rel_path)};
  std::cout << "Loading config: " << config_path << std::endl;
  {
    std::ifstream t(config_path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::cout << buffer.str();
  }

  SchedulerConfig cfg;
  cfg.parse(config_path);
  // std::cout << cfg.help() << std::endl;
  cfg.validate();
  constexpr bool dry_run = true;
  constexpr bool in_bazel_test = true;
  constexpr bool mock_kafka = true;
  m_scheduler.reset(new Scheduler{cfg, dry_run, in_bazel_test, mock_kafka}); //

  REQUIRE(m_scheduler->m_loaded_uts_count != 0);
  REQUIRE(m_scheduler->m_url_freq_stats->max() != 0);
}

} // namespace contentv1
