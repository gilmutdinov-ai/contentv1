#include "bin/contentd/cli/Contentd.h"
#include "misc/Log.h"
#include "misc/kafkawrap/KafkaStaticSim.h"
#include "scylla/PageDbScylla.h"
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace contentv1 {

ContentdConfig::ContentdConfig() { addStrings(s_cfg_strings); }

int Contentd::operator()(int argc, const char **argv) {

  CliAction cli_action = _parseArgs(argc, argv);
  if (cli_action == CliAction::EXIT_OK)
    return 0;
  else if (cli_action == CliAction::PARSING_ERROR)
    return 1;

  ContentdConfig contentd_config;
  try {
    LOG("Parsing config: " << m_config_path);
    contentd_config.parseFile(m_config_path);
    contentd_config.validate("Contentd::Contentd");
  } catch (std::exception &_ex) {
    LOG(_ex.what());
    return 1;
  }
  LOG("Config parsed successfully");
  if (m_is_dry_run)
    LOG("performing dry run..");
  if (m_is_under_bazel_test)
    LOG("running under bazel test..");

  // SCHEDULER
  LOG("Init scheduler..");
  constexpr bool mock_kafka = false;
  m_scheduler.reset(new Scheduler{contentd_config, m_is_dry_run,
                                  m_is_under_bazel_test, mock_kafka});
  LOG("Init scheduler..DONE");

  if (mock_kafka) {
    // Waiting for Scheduler to push something to queue (KafkaStaticSim)
    // maybe remove
    sleep(2);
    LOG("Records in queue: " << misc::KafkaStaticSim::size(
            contentd_config[EnqueueLoopConfig::STR_KAFKA_PUSH_TOPIC]
                .as<std::string>()));
  }

  // PAGEDB
  std::string page_db_impl{
      contentd_config[ContentdConfig::STR_PAGE_DB_IMPL].as<std::string>()};
  if ("ldb" == page_db_impl) {
    LOG("Init page_db ldb");
    m_page_db.reset(new PageDb{contentd_config});
  } else if ("scylla" == page_db_impl) {
    LOG("Init page_db_scylla");
    YAML::Emitter emitter;
    emitter << contentd_config["page_db_scylla"];
    PageDbScyllaConfig scylla_sub;
    scylla_sub.parseString(emitter.c_str());
    scylla_sub.validate();
    m_page_db.reset(new PageDbScylla{scylla_sub});
  } else {
    LOG("Unknown page db impl: " << page_db_impl);
    throw std::runtime_error("Unknown page db impl");
  }

  // WORKER
  LOG("Init worker..");
  m_worker.reset(
      new Worker{m_page_db, m_scheduler, contentd_config, m_is_dry_run});
  LOG("Init worker..DONE");

  // API
  m_api_v1_w.reset(new APIv1Wrap{m_page_db, contentd_config});

  if (m_is_dry_run) {

    // sleep(7);
    m_sched_stats = m_worker->getStats();
    LOG("Worker fetch pushed:" << m_sched_stats.pushed);
    LOG("Worker fetch successes:" << m_sched_stats.success);
    LOG("Worker fetch fails:" << m_sched_stats.fail);
    LOG("Worker fetch robots successes:" << m_sched_stats.robots_success);
    LOG("Worker fetch robots fails:" << m_sched_stats.robots_fail);

    // check on scheduler stats
    m_fetch_stats = m_scheduler->getStats();
    LOG("Sched fetch successes:" << m_fetch_stats.success);
    LOG("Sched fetch fails:" << m_fetch_stats.fail);
    LOG("Sched fetch robots successes:" << m_fetch_stats.robots_success);
    LOG("Sched fetch robots fails:" << m_fetch_stats.robots_fail);

    int ret_code = (isStatsEqual() && isStatsAnyOk()) ? 0 : 1;

    LOG("finishing dry run, exiting, hello destructors");
    return ret_code;
  }

  m_scheduler->start();
  m_worker->start();

  m_scheduler->join();
  m_worker->join();

  return 0;
}

bool Contentd::isStatsEqual() const { return m_sched_stats == m_fetch_stats; }

bool Contentd::isStatsAnyOk() const {
  return m_sched_stats.success != 0 || m_sched_stats.robots_success != 0;
}

namespace po = boost::program_options;

// _parseArgs
//   sets m_is_dry_run, m_config_path
//
Contentd::CliAction Contentd::_parseArgs(int argc, const char **argv) {

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
    return CliAction::EXIT_OK;
  }

  if (vm.count("config_help")) {
    LOG(ContentdConfig{}.help());
    LOG("Example: src/bin/contentd/ut/contentd_ut.conf");
    return CliAction::EXIT_OK;
  }

  m_is_dry_run = false;
  if (vm.count("dry_run")) {
    m_is_dry_run = true;
    return CliAction::DRY_RUN;
  }

  m_is_under_bazel_test = false;
  if (vm.count("bazel_test")) {
    m_is_under_bazel_test = true;
  }

  return CliAction::NORMAL_RUN;
}

} // namespace contentv1
