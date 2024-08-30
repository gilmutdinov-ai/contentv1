#include "GFQRecord.h"
#include <stdexcept>

namespace contentv1 {

GFQRecord::GFQRecord(const std::string &_url, Ts _attempt_ts)
    : url(_url), attempt_ts(_attempt_ts) {}

GFQRecord::GFQRecord(const std::string &_dump) {
  GFQRecordPb pb;
  if (!pb.ParseFromString(_dump))
    throw std::invalid_argument("GFQRecord::GFQRecord parsing");
  GFQRecord::fromPb(*this, pb);
}

// GFQRecord::GFQRecord(const GFQRecordPb &_pb) { fromPb_(_pb); }

void GFQRecord::fromPb(GFQRecord &_, const GFQRecordPb &_pb) {
  _.url = _pb.url();
  _.attempt_ts = _pb.attempt_ts();
}

GFQRecord GFQRecord::fromPb(const GFQRecordPb &_pb) {
  GFQRecord ret;
  GFQRecord::fromPb(ret, _pb);
  return ret;
}

void GFQRecord::toPb(GFQRecordPb &_pb) const {
  _pb.set_url(url);
  _pb.set_attempt_ts(attempt_ts);
}

#warning CHECK_THIS_WORKS
void GFQRecord::toPb(GFQRecordPb *_pb) const { toPb(*_pb); }

std::string GFQRecord::dump() const {
  GFQRecordPb pb;
  toPb(pb);
  return pb.SerializeAsString();
}

bool GFQRecord::isRobots() const {
  return url.ends_with(GFQRecord::S_ROBOTS_POSTFIX);
}

bool GFQRecord::operator<(const GFQRecord &_a) const { return url < _a.url; }

void print(const std::vector<GFQRecord> &_v) {
  for (size_t i = 0; i < _v.size(); ++i)
    std::cout << _v[i].url << std::endl;
}

} // namespace contentv1
