#pragma once

#include "api/v1/APIv1Wrap.h"
#include "scheduler/Scheduler.h"
#include "scheduler/rpc/SchedulerServer.h"
#include "scylla/PageDbScylla.h"

namespace contentv1 {

class SchedConfig : public virtual SchedulerConfig,
                    public virtual PageDbScyllaConfig,
                    public virtual APIv1WrapConfig {
public:
};

class Sched {

  enum CliAction {
    EXIT_OK = 0,
    PARSING_ERROR = 1,
    DRY_RUN = 2,
    NORMAL_RUN = 3
  };

  std::string m_config_path = "src/bin/sched/cli/ut/sched_ut.conf";
  bool m_is_dry_run = false;
  bool m_is_under_bazel = false;

  // SCHEDULER
  Scheduler::Ptr m_scheduler;

  // SCHEDULER SERVER (RPC)
  SchedulerServer::Ptr m_scheduler_server;

  // PAGE DB (SCYLLA)
  IPageDb::Ptr m_page_db;

  // STATS
  FetchStats m_sched_stats;
  FetchStats m_fetch_stats;

  // API (QueryLoop)
  APIv1Wrap::Ptr m_api_v1_w;

public:
  int operator()(int argc, char **argv);

  bool isStatsEqual() const;
  bool isStatsAnyOk() const;

  ~Sched() { LOG("~Sched() start/finish"); }

private:
  CliAction _parseArgs(int argc, char **argv);
};
} // namespace contentv1
