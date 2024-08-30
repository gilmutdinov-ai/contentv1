#pragma once
#include "core/GFQRecord.h"
#include "src/proto/CrawlAttempts.pb.h"
#include "src/proto/FetchResult.pb.h"
#include <string>

/*
FetchResult воркер передает шедулеру,
    в случае роботс response лежит текст роботс, иначе пустой
*/

namespace contentv1 {

struct FetchResult {
  GFQRecord gfq;
  CrawlAttemptsPb::AttemptStatus status;
  std::string error_str;
  std::string content;

  static FetchResult fromPb(const FetchResultPb &_pb);

  void toPb(FetchResultPb &_pb) const;
  void toPb(FetchResultPb *_pb) const;

  bool isOk() const;
  bool isRobots() const;
};

} // namespace contentv1
