#include "bin/workd/cli/Workd.h"
#include "misc/Log.h"
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace contentv1 {

Workd::~Workd() { LOG("~Workd() start/finish"); }

int Workd::operator()(int argc, char **argv) {

  CliAction cli_action = _parseArgs(argc, argv);
  if (cli_action == EXIT_OK)
    return 0;
  else if (cli_action == PARSING_ERROR)
    return 1;

#warning ALLOW_DRY_RUN_FROM_UT_BAZEL

  WorkdConfig workd_config;
  try {
    LOG("Parsing config: " << m_config_path);
    workd_config.parse(m_config_path);
    workd_config.validate("Workd::Workd");
  } catch (std::exception &_ex) {
    LOG(_ex.what());
    return 1;
  }
  LOG("Config parsed successfully");
  if (m_is_dry_run)
    LOG("performing dry run..");
  if (m_is_under_bazel)
    LOG("running under bazel..");

  // PAGE DB SCYLLA
  m_page_db.reset(new PageDbScylla{workd_config});

  // SCHEDULER CLEINT (RPC)
#warning IMPLEMENT
  // WORKER
#warning IMPLEMENT
  /*
  LOG("Init scheduler..");
  m_scheduler.reset(
      new Scheduler{sched_config, m_is_dry_run, m_is_under_bazel});
  LOG("Init scheduler..DONE");
  */
  /*
    if (m_is_dry_run) {
      // Waiting for Scheduler to push something to queue (KafkaStaticSim)
      // maybe remove
      sleep(2);
      LOG("Records in queue: " << misc::KafkaStaticSim::size(
              sched_config[EnqueueLoopConfig::STR_KAFKA_PUSH_TOPIC].asString()));
    }
  */

  /*
    if (m_is_dry_run) {

      // check on scheduler stats
      m_fetch_stats = m_scheduler->getStats();
      LOG("Workd fetch successes:" << m_fetch_stats.success);
      LOG("Workd fetch fails:" << m_fetch_stats.fail);
      LOG("Workd fetch robots successes:" << m_fetch_stats.robots_success);
      LOG("Workd fetch robots fails:" << m_fetch_stats.robots_fail);

      int ret_code = (isStatsEqual() && isStatsAnyOk()) ? 0 : 1;

      LOG("finishing dry run, exiting, hello destructors");
      return ret_code;
    }
  */
  m_worker->start();
  m_worker->join();

  return 0;
}

bool Workd::isStatsEqual() const { return m_sched_stats == m_fetch_stats; }

bool Workd::isStatsAnyOk() const {
  return m_sched_stats.success != 0 || m_sched_stats.robots_success != 0;
}

namespace po = boost::program_options;

// _parseArgs
//   sets m_is_dry_run, m_config_path
//
Workd::CliAction Workd::_parseArgs(int argc, char **argv) {

  po::options_description desc("General options");
  std::string task_type;
  desc.add_options()("help,h", "Show help")("dry_run,d", "Dry run")(
      "bazel_test,b", "Under bazel test")("config_help,ch", "Show config help")(
      "config,c", po::value<std::string>(&m_config_path), "Path to config");

  po::variables_map vm;
  po::parsed_options parsed = po::command_line_parser(argc, argv)
                                  .options(desc)
                                  .allow_unregistered()
                                  .run();
  po::store(parsed, vm);
  po::notify(vm);

  if (vm.count("help")) {
    LOG(desc);
    return EXIT_OK;
  }

  if (vm.count("config_help")) {
    LOG(WorkdConfig{}.help());
    LOG("Example: src/bin/sched/ut/sched_ut.conf");
    return EXIT_OK;
  }

  m_is_dry_run = false;
  if (vm.count("dry_run")) {
    m_is_dry_run = true;
    return DRY_RUN;
  }

  m_is_under_bazel = false;
  if (vm.count("bazel_test")) {
    m_is_under_bazel = true;
  }

  return NORMAL_RUN;
}

} // namespace contentv1
