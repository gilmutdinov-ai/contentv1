#include "kafkawrap/KafkaReader.h"
#include <iostream>

#include "simdjson.h"

using namespace contentv1;
using namespace simdjson;

int main(int argc, char **argv) {

  // std::string topic{"req_urls_stream"};
  std::string topic{"test_topic"};
  std::vector<std::string> brokers{"kafka"};
  std::string group_id{"test_kafka"};
  long long limit{10};
  KafkaReader reader{topic, brokers, group_id};

  ondemand::parser parser;
  reader.read(
      [&](const std::string &mess) {
        std::cout << mess << std::endl;
        simdjson::padded_string json{mess};
        auto doc = parser.iterate(json);
        std::string_view url = doc["url"];
        uint64_t ts = doc["ts"];
        std::cout << "url: " << url << std::endl;
        std::cout << "ts: " << ts << std::endl;
      },
      limit);

  return 0;
}