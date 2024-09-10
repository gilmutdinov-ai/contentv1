#include "scheduler/rpc/SchedulerServerCallDataMethods.h"

namespace contentv1 {

CallDataTryFetchUrls::CallDataTryFetchUrls(SchedulerRPC::AsyncService *service,
                                           grpc::ServerCompletionQueue *cq,
                                           IScheduler::Ptr _sched)
    : CallDataBase{service, cq, _sched}, responder_(&ctx_) {
  Proceed();
}

void CallDataTryFetchUrls::Proceed() {
  if (status_ == CREATE) {
    status_ = PROCESS;

    //// CREATE REQUEST
    service_->RequestTryFetchUrls(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
  } else if (status_ == PROCESS) {

    new CallDataTryFetchUrls(service_, cq_, m_sched);
    //// PROCESS REQUEST
    CallTryFetchUrls(request_, reply_);

    status_ = FINISH;
    responder_.Finish(reply_, grpc::Status::OK, this);
  } else {

    delete this;
  }
}

void CallDataTryFetchUrls::CallTryFetchUrls(const TryFetchRequest &_request,
                                            TryFetchReply &_reply) {

  // PARSE try_urls FROM REQUEST
  std::vector<GFQRecord> try_urls;

  const auto &g_try_urls = _request.try_urls();
  for (size_t i = 0; i < g_try_urls.size(); ++i) {
    auto gfq_pb = g_try_urls[i];
    try_urls.push_back(GFQRecord::fromPb(gfq_pb));
  }

  // CALL SCHEDULER
  std::vector<GFQRecord> allowed_urls;
  m_sched->tryFetchUrls(try_urls, allowed_urls);

  // WRITE allowed_urls TO REPLY
  for (const auto &gfq : allowed_urls) {
    auto g_gfq = _reply.add_allowed_urls();
    gfq.toPb(g_gfq);
  }
}

} // namespace contentv1