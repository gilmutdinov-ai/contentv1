#pragma once
#include "scheduler/IScheduler.h"

#include "src/scheduler/rpc/scheduler.grpc.pb.h"
#include "src/scheduler/rpc/scheduler.pb.h"

#include <grpcpp/grpcpp.h>

namespace contentv1 {

class CallDataBase {
public:
  CallDataBase(SchedulerRPC::AsyncService *service,
               grpc::ServerCompletionQueue *cq, IScheduler::Ptr _sched);
  virtual ~CallDataBase() = default;
  virtual void Proceed() = 0;
  //  void CallTryFetchUrls(const TryFetchRequest &_request, TryFetchReply
  //  &_reply);

protected:
  SchedulerRPC::AsyncService *service_;
  grpc::ServerCompletionQueue *cq_;
  grpc::ServerContext ctx_;

  IScheduler::Ptr m_sched;

  // in ancestors:
  /*
    TryFetchRequest request_;
    TryFetchReply reply_;
    grpc::ServerAsyncResponseWriter<TryFetchReply> responder_;
  */

  enum CallStatus { CREATE, PROCESS, FINISH };
  CallStatus status_;
};

} // namespace contentv1
