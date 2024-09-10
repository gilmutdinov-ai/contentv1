#include "SchedulerServer.h"
#include "misc/Log.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

namespace contentv1 {

SchedulerServerConfig::SchedulerServerConfig() {
  addInts(s_cfg_ints);
  addStrings(s_cfg_strings);
}

SchedulerServer::SchedulerServer(const SchedulerServerConfig &_config,
                                 IScheduler::Ptr _sched)
    : m_listen_addr{_config[SchedulerServerConfig::STR_LISTEN_ADDR].asString()},
      m_listen_port{_config[SchedulerServerConfig::INT_LISTEN_PORT].asInt()},
      m_sched(_sched), m_th{std::bind(&SchedulerServer::_thread, this)} {}

void SchedulerServer::join() {
  if (m_th.joinable())
    m_th.join();
}

void SchedulerServer::stop() {
  if (!m_running.load())
    return;
  server_->Shutdown();
  cq_->Shutdown();
  m_running.store(false);
}

SchedulerServer::~SchedulerServer() {
  stop();
  join();
}

// returns when m_running==false
void SchedulerServer::_thread() {

  std::string server_address =
      absl::StrFormat("%s:%d", m_listen_addr, m_listen_port);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service_);
  cq_ = builder.AddCompletionQueue();
  server_ = builder.BuildAndStart();
  LOG("Server listening on " << server_address);

  HandleRpcs();
}

void SchedulerServer::HandleRpcs() {
  // add other methods in here
  InstateAllCallDatas(cq_, service_, m_sched);

  void *tag; // uniquely identifies a request.
  bool ok;
  while (m_running.load()) {
    cq_->Next(&tag, &ok); // exit here on server->shutdown
    // REQUIRE(ok);
    if (m_running.load() && ok)
      static_cast<CallDataBase *>(tag)->Proceed();
  }
}

} // namespace contentv1
