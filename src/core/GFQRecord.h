#pragma once
#include "core/Types.h"
#include "src/proto/GFQRecord.pb.h"

namespace contentv1 {

class GFQRecord {
  inline const static std::string S_ROBOTS_POSTFIX{"/robots.txt"};

public:
  std::string url;
  Ts attempt_ts;

  GFQRecord() = default;
  GFQRecord(const std::string &_url, Ts _attempt_ts);
  GFQRecord(const std::string &_dump);
  // GFQRecord(const GFQRecordPb &_pb);
  std::string dump() const;
  bool isRobots() const;

  bool operator<(const GFQRecord &_a) const;

  void toPb(GFQRecordPb &_pb) const;
  void toPb(GFQRecordPb *_pb) const;

  static GFQRecord fromPb(const GFQRecordPb &_pb);
  static void fromPb(GFQRecord &_, const GFQRecordPb &_pb);
};

void print(const std::vector<GFQRecord> &_v);

} // namespace contentv1

namespace std {
template <> struct less<contentv1::GFQRecord> {
  bool operator()(const contentv1::GFQRecord &a,
                  const contentv1::GFQRecord &b) const {
    return a.url < b.url;
  }
};
} // namespace std