#include "scheduler/rpc/SchedulerServer.h"

namespace contentv1 {
CallDataBase::CallDataBase(SchedulerRPC::AsyncService *service,
                           grpc::ServerCompletionQueue *cq,
                           IScheduler::Ptr _sched)
    : service_(service), cq_(cq), m_sched(_sched), status_(CREATE) {}

} // namespace contentv1
