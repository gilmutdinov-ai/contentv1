#include "FetchResult.h"

namespace contentv1 {

FetchResult FetchResult::fromPb(const FetchResultPb &_pb) {
  FetchResult ret;
  ret.gfq = GFQRecord::fromPb(_pb.gfq());
  ret.status = _pb.status();
  ret.error_str = _pb.error_str();
  ret.content = _pb.content();
  return ret;
}

void FetchResult::toPb(FetchResultPb &_pb) const {

  GFQRecordPb *gfq_pb = _pb.mutable_gfq();
  gfq.toPb(gfq_pb);

  _pb.set_status(status);
  _pb.set_error_str(error_str);
  _pb.set_content(content);
}

void FetchResult::toPb(FetchResultPb *_pb) const { toPb(*_pb); }

bool FetchResult::isOk() const {
  return status == CrawlAttemptsPb::ATTEMPT_STATUS_OK;
}

bool FetchResult::isRobots() const { return gfq.isRobots(); }

} // namespace contentv1
