#include "kafkawrap/GFQReader.h"

namespace contentv1 {
void GFQReader::read(misc::KafkaReaderI::Ptr _kafka_reader, bool _dont_block,
                     std::function<void(const GFQRecord &_rec)> _cb,
                     long long _limit) {
  _kafka_reader->read(
      _dont_block,
      [&](const std::string &_proto_str) {
        GFQRecord gfq(_proto_str);
        _cb(gfq);
      },
      _limit);
}
} // namespace contentv1