#include "scheduler/rpc/SchedulerServerCallDataOnFetched.h"

namespace contentv1 {

CallDataOnFetched::CallDataOnFetched(SchedulerRPC::AsyncService *service,
                                     grpc::ServerCompletionQueue *cq,
                                     IScheduler::Ptr _sched)
    : CallDataBase{service, cq, _sched}, responder_(&ctx_) {
  Proceed();
}

void CallDataOnFetched::Proceed() {
  if (status_ == CREATE) {
    status_ = PROCESS;

    //// CREATE REQUEST
    service_->RequestOnFetched(&ctx_, &request_, &responder_, cq_, cq_, this);
  } else if (status_ == PROCESS) {

    new CallDataOnFetched(service_, cq_, m_sched);
    //// PROCESS REQUEST
    CallOnFetched(request_, reply_);

    status_ = FINISH;
    responder_.Finish(reply_, grpc::Status::OK, this);
  } else {

    delete this;
  }
}

void CallDataOnFetched::CallOnFetched(const OnFetchedRequest &_request,
                                      OnFetchedReply &_reply) {
  // PARSE results FROM REQUEST
  std::vector<FetchResult> results;

  const auto &g_results = _request.results();
  for (size_t i = 0; i < g_results.size(); ++i) {
    auto result_pb = g_results[i];
    results.push_back(FetchResult::fromPb(result_pb));
  }

  // CALL SCHEDULER
  m_sched->onFetched(results);
}

} // namespace contentv1