#pragma once
#include "scheduler/rpc/SchedulerServerCallData.h"

namespace contentv1 {

class CallDataOnFetched final : public CallDataBase {
public:
  CallDataOnFetched(SchedulerRPC::AsyncService *service,
                    grpc::ServerCompletionQueue *cq, SchedulerApi::Ptr _sched);

  void Proceed() override;
  virtual ~CallDataOnFetched() = default;

private:
  void CallOnFetched(const OnFetchedRequest &_request, OnFetchedReply &_reply);

  OnFetchedRequest request_;
  OnFetchedReply reply_;
  grpc::ServerAsyncResponseWriter<OnFetchedReply> responder_;
};

} // namespace contentv1