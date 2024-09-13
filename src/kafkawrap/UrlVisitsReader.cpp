#include "UrlVisitsReader.h"
#include "simdjson.h"
#include <iostream>

namespace contentv1 {

void UrlVisitsReader::read(misc::KafkaReaderI::Ptr _kafka_reader,
                           bool _dont_block,
                           std::function<void(const UrlVisit &url_visit)> _cb,
                           long long _limit) {
  simdjson::ondemand::parser parser;
  _kafka_reader->read(
      _dont_block,
      [&](const std::string &mess) {
        simdjson::padded_string json{mess};
        auto doc = parser.iterate(json);
        std::string_view url = doc["url"];
        uint64_t ts = doc["ts"];
        // std::cout << "UrlVisitsReader.cpp: ts: " << ts << std::endl;
        std::chrono::system_clock::time_point tp{std::chrono::seconds{ts}};
        // std::cout << "created time_point" << std::endl;
        UrlVisit visit{Url{url}, tp};
        // std::cout << "created UrlVisit" << std::endl;
        _cb(visit);
      },
      _limit);
}
} // namespace contentv1
