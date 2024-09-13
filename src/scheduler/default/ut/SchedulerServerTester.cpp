#include "SchedulerServerTester.h"
#include "kafkawrap/GFQReader.h"
#include "misc/Bazel.h"
#include "misc/kafkawrap/KafkaReaderMock.h"
#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace contentv1 {

SchedulerServerTester::SchedulerServerTester() {

  // INIT SCHEDULER
  std::filesystem::remove_all("/tmp/sched_srv_ut");

  auto config_path{misc::get_bazel_test_data_path(s_conf_rel_path)};
  std::cout << "Loading config: " << config_path << std::endl;
  {
    std::ifstream t(config_path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::cout << buffer.str();
  }

  m_cfg.parse(config_path);
  // std::cout << cfg.help() << std::endl;
  m_cfg.validate();
  {
    constexpr bool dry_run = true;
    constexpr bool in_bazel_test = true;
    constexpr bool kafka_mock = true;
    m_scheduler.reset(
        new Scheduler{m_cfg, dry_run, in_bazel_test, kafka_mock}); //
  }

  // INIT SERVER
  s_port = s_port_start + rand() % s_port_rnd_range;
  m_cfg[SchedulerServerConfig::INT_LISTEN_PORT] = s_port;

  m_sched_server.reset(new SchedulerServer{m_cfg, m_scheduler});

  // m_server_th.reset(new std::thread{
  //     [&]() { m_sched_server->Run(s_port); }}); // while m_running

  sleep(1);
  auto allowed_urls = callTryFetch_(); // CALL TryFetch FROM CLIENT
  REQUIRE(allowed_urls.size() != 0);

  REQUIRE(m_scheduler->m_loaded_uts_count != 0);
  REQUIRE(m_scheduler->m_url_freq_stats->max() != 0);

  callOnFetched_(allowed_urls); // CALL OnFetched FROM CLIENT

  // Check Scheduler state after
  REQUIRE(m_scheduler->m_crawled_db->getFinishedToday() == allowed_urls.size());

  // m_sched_server->stop();
  // m_sched_server->join();
}

void SchedulerServerTester::callOnFetched_(
    const std::vector<GFQRecord> &_allowed) {

  std::cout << "SchedulerServerTester::callOnFetched_\n";
  std::string target_str =
      "127.0.0.1:" + std::to_string(SchedulerServerTester::s_port);
  SchedulerClient client(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  std::vector<FetchResult> results;
  for (auto gfq : _allowed) {
    FetchResult result{
        gfq, CrawlAttemptsPb::ATTEMPT_STATUS_OK,
        "", // error_str
        ""  // content
    };
    results.push_back(result);
  }
  client.onFetched(results);
}

std::vector<GFQRecord> SchedulerServerTester::callTryFetch_() {

  std::cout << "SchedulerServerTester::callTryFetch_\n";
  std::string target_str =
      "127.0.0.1:" + std::to_string(SchedulerServerTester::s_port);
  SchedulerClient client(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  std::vector<GFQRecord> try_urls;
  //
  misc::KafkaReaderMock::Ptr m_kafka_reader;
  m_kafka_reader.reset(new misc::KafkaReaderMock{
      m_cfg[EnqueueLoopConfig::STR_KAFKA_PUSH_TOPIC].asString()});
  GFQReader::read(
      m_kafka_reader, true, // dont block
      [&](const GFQRecord &_gfq) { try_urls.push_back(_gfq); }, 10);
  //
  std::cout << "try_urls: " << try_urls.size() << std::endl;
  std::vector<GFQRecord> allowed_urls;
  std::cout << "rpc calling tryFetchUrlsSync..\n";
  client.tryFetchUrls(try_urls, allowed_urls);
  std::cout << "got allowed_urls: " << allowed_urls.size() << std::endl;
  return allowed_urls;
}

} // namespace contentv1
