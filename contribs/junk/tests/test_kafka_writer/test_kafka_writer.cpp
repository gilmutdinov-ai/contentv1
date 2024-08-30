#include "kafkawrap/KafkaReader.h"
#include "kafkawrap/KafkaWriter.h"
#include <iostream>
#include <thread>

#include "simdjson.h"

using namespace contentv1;
using namespace simdjson;

int main(int argc, char **argv) {

  std::cout << "test_kafka_writer" << std::endl;

  std::string topic{"test_topic"};
  std::vector<std::string> brokers{"kafka"};
  /*
    std::thread th([&]() {
      std::cout << "starting reader..\n";
      KafkaReader reader{topic, brokers};

      sleep(1);
      std::cout << "trying to read..\n";
      reader.read([&](const std::string &mess) {
        std::cout << "Read back message: " << mess << std::endl;
      });
      exit(0);
    });
    sleep(1);
  */
  std::cout << "starting writer..\n";
  // std::string topic{"req_urls_stream"};
  KafkaWriter writer{brokers};
  for (size_t i = 0; i < 3; ++i)
    writer.write(topic, "_KafkaWriter_message_");
  writer.flush(3000);

  sleep(1);
  // th.join();
  return 0;
}