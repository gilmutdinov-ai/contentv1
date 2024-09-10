#pragma once
#include "scheduler/default/Scheduler.h"
#include "scheduler/rpc/SchedulerClient.h"
#include "scheduler/rpc/SchedulerServer.h"

namespace contentv1 {

class SchedulerServerTesterConfig : virtual public SchedulerConfig,
                                    virtual public SchedulerServerConfig {
public:
  virtual ~SchedulerServerTesterConfig() {}
};

class SchedulerServerTester {

  inline static const std::string s_conf_rel_path{
      "/src/scheduler/default/ut/scheduler_srv_ut.conf"};

  inline static uint16_t s_port;
  inline static uint16_t s_port_start = 8083;
  inline static uint16_t s_port_rnd_range = 100;

  SchedulerServerTesterConfig m_cfg;
  Scheduler::Ptr m_scheduler;
  SchedulerServer::Ptr m_sched_server;

  // std::unique_ptr<std::thread> m_server_th;

  std::vector<GFQRecord> callTryFetch_();
  void callOnFetched_(const std::vector<GFQRecord> &_allowed);

public:
  SchedulerServerTester();
};

} // namespace contentv1