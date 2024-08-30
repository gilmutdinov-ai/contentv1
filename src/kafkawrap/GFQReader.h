#pragma once

#include "core/GFQRecord.h"
#include "core/Types.h"
#include "misc/kafkawrap/KafkaReader.h"

namespace contentv1 {
class GFQReader {
public:
  static void read(misc::KafkaReaderI::Ptr _kafka_reader,
                   std::function<void(const GFQRecord &_rec)> _cb,
                   long long _limit = -1);
};
} // namespace contentv1
