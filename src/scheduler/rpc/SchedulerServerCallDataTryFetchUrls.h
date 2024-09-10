#pragma once
#include "scheduler/rpc/SchedulerServerCallData.h"

namespace contentv1 {

class CallDataTryFetchUrls final : public CallDataBase {
public:
  CallDataTryFetchUrls(SchedulerRPC::AsyncService *service,
                       grpc::ServerCompletionQueue *cq, IScheduler::Ptr _sched);

  virtual ~CallDataTryFetchUrls() = default;
  void Proceed() override;

private:
  void CallTryFetchUrls(const TryFetchRequest &_request, TryFetchReply &_reply);

  TryFetchRequest request_;
  TryFetchReply reply_;
  grpc::ServerAsyncResponseWriter<TryFetchReply> responder_;
};

} // namespace contentv1