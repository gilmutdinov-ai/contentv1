#pragma once

#include "scheduler/IScheduler.h"

#include "scheduler/rpc/SchedulerServerCallData.h"
#include "src/scheduler/rpc/scheduler.grpc.pb.h"
#include "src/scheduler/rpc/scheduler.pb.h"

#include <grpcpp/grpcpp.h>

#include "misc/Config.h"

#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace contentv1 {

class SchedulerServerConfig : virtual public misc::Config {
public:
  inline static const std::string STR_LISTEN_ADDR{"sched_server_listen_addr"};
  inline static const std::string INT_LISTEN_PORT{"sched_server_listen_port"};

private:
  const std::vector<std::string> s_cfg_strings = {STR_LISTEN_ADDR};
  const std::vector<std::string> s_cfg_ints = {INT_LISTEN_PORT};

public:
  SchedulerServerConfig();
  virtual ~SchedulerServerConfig() = default;
};

class SchedulerServer final {
public:
  using Ptr = std::shared_ptr<SchedulerServer>;

  SchedulerServer(const SchedulerServerConfig &_config, IScheduler::Ptr _sched);
  ~SchedulerServer();

  void stop();
  void join();

private:
  void _thread();
  void HandleRpcs();
  void InstateAllCallDatas(std::unique_ptr<grpc::ServerCompletionQueue> &_cq,
                           SchedulerRPC::AsyncService &_service,
                           IScheduler::Ptr _sched) const;

  const std::string m_listen_addr;
  const int m_listen_port;

  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
  SchedulerRPC::AsyncService service_;
  std::unique_ptr<grpc::Server> server_;

  IScheduler::Ptr m_sched;

  std::thread m_th;
  std::atomic<bool> m_running{true};
};

} // namespace contentv1
