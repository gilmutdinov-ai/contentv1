#pragma once

#include "scheduler/rpc/SchedulerClient.h"
#include "scylla/PageDbScylla.h"
#include "worker/Worker.h"

namespace contentv1 {

class WorkdConfig : public virtual WorkerConfig,
                    public virtual PageDbScyllaConfig {
public:
  inline static const std::string STR_SCHED_GRPC_TARGET_STR{
      "sched_grpc_target_str"};

  WorkdConfig();
  virtual ~WorkdConfig() = default;
};

class Workd {

  enum CliAction {
    EXIT_OK = 0,
    PARSING_ERROR = 1,
    DRY_RUN = 2,
    NORMAL_RUN = 3
  };

  std::string m_config_path = "src/bin/workd/cli/ut/workd_ut.conf";
  bool m_is_dry_run = false;
  bool m_is_under_bazel_test = false;

  // PAGE DB (SCYLLA)
  IPageDb::Ptr m_page_db;

  // SCHEDULER CLEINT (RPC)
  SchedulerClient::Ptr m_scheduler_client;

  // WORKER
  Worker::Ptr m_worker;

  // STATS
  FetchStats m_sched_stats;
  FetchStats m_fetch_stats;

public:
  int operator()(int argc, char **argv);

  bool isStatsEqual() const;
  bool isStatsAnyOk() const;

  ~Workd();

private:
  CliAction _parseArgs(int argc, char **argv);
};
} // namespace contentv1
