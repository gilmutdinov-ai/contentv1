#include "scheduler/rpc/SchedulerServerCallDataMethods.h"
#include "scheduler/rpc/SchedulerServer.h"

namespace contentv1 {

void SchedulerServer::InstateAllCallDatas(
    std::unique_ptr<grpc::ServerCompletionQueue> &_cq,
    SchedulerRPC::AsyncService &_service, IScheduler::Ptr _sched) const {

  new CallDataTryFetchUrls(&_service, _cq.get(), _sched);
  new CallDataOnFetched(&_service, _cq.get(), _sched);
}

} // namespace contentv1
