#pragma once

#include "core/Types.h"
#include "core/UrlVisit.h"
#include "misc/kafkawrap/KafkaReader.h"

namespace contentv1 {
class UrlVisitsReader {
public:
  static void read(misc::KafkaReaderI::Ptr _kafka_reader, bool _dont_block,
                   std::function<void(const UrlVisit &url_visit)> _cb,
                   long long _limit = -1);
};
} // namespace contentv1
