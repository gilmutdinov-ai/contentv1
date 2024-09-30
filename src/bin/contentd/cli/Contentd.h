#pragma once

#include "api/v1/APIv1Wrap.h"
#include "scheduler/default/Scheduler.h"
#include "worker/Worker.h"

namespace contentv1 {

class ContentdConfig : public virtual PageDbConfig,
                       public virtual SchedulerConfig,
                       public virtual WorkerConfig,
                       public virtual APIv1WrapConfig {
public:
  inline static const std::string STR_PAGE_DB_IMPL{"page_db_impl"};

private:
  const std::vector<std::string> s_cfg_strings = {STR_PAGE_DB_IMPL};

public:
  ContentdConfig();
  virtual ~ContentdConfig() = default;
};

class Contentd {

  enum class CliAction {
    EXIT_OK = 0,
    PARSING_ERROR = 1,
    DRY_RUN = 2,
    NORMAL_RUN = 3
  };

  std::string m_config_path = "src/bin/contentd/cli/ut/contentd_ut.conf";
  bool m_is_dry_run = false;
  bool m_is_under_bazel_test = false;

  // SCHEDULER
  Scheduler::Ptr m_scheduler;

  // PAGEDB
  IPageDb::Ptr m_page_db;

  // WORKER
  FetchLoop::Ptr m_fetch_loop;
  Worker::Ptr m_worker;

  // STATS
  FetchStats m_sched_stats;
  FetchStats m_fetch_stats;

  // API (QueryLoop)
  APIv1Wrap::Ptr m_api_v1_w;

public:
  int operator()(int argc, char **argv);

  bool isStatsEqual() const;
  bool isStatsAnyOk() const;

  ~Contentd() { LOG("~Contentd() start/finish"); }

private:
  CliAction _parseArgs(int argc, char **argv);

  friend class Contentd;
};
} // namespace contentv1
