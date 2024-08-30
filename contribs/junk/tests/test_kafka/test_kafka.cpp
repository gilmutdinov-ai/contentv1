#include <librdkafka/rdkafkacpp.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#ifndef _WIN32
#include <sys/time.h>
#endif

#ifdef _WIN32
#include "../win32/wingetopt.h"
#include <atltime.h>
#elif _AIX
#include <unistd.h>
#else
#include <getopt.h>
#include <unistd.h>
#endif

static volatile sig_atomic_t run = 1;

static void sigterm(int sig) { run = 0; }

static int64_t now() {
#ifndef _WIN32
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((int64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#else
#error "now() not implemented for Windows, please submit a PR"
#endif
}

static std::vector<RdKafka::Message *>
consume_batch(RdKafka::KafkaConsumer *consumer, size_t batch_size,
              int batch_tmout) {
  std::vector<RdKafka::Message *> msgs;
  msgs.reserve(batch_size);

  int64_t end = now() + batch_tmout;
  int remaining_timeout = batch_tmout;

  while (msgs.size() < batch_size) {
    RdKafka::Message *msg = consumer->consume(remaining_timeout);

    switch (msg->err()) {
    case RdKafka::ERR__TIMED_OUT:
      delete msg;
      return msgs;

    case RdKafka::ERR_NO_ERROR:
      msgs.push_back(msg);
      break;

    default:
      std::cerr << "%% Consumer error: " << msg->errstr() << std::endl;
      run = 0;
      delete msg;
      return msgs;
    }

    remaining_timeout = end - now();
    if (remaining_timeout < 0)
      break;
  }

  return msgs;
}

int main(int argc, char **argv) {

  std::string errstr;
  std::string topic_str;
  std::vector<std::string> topics{"req_urls_stream"};
  int batch_size = 10;
  int batch_tmout = 5000;

  RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

  if (conf->set("enable.partition.eof", "false", errstr) !=
      RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }

  if (conf->set("group.id", "test_kafka", errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }

  if (conf->set("metadata.broker.list", "kafka", errstr) !=
      RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }

  signal(SIGINT, sigterm);
  signal(SIGTERM, sigterm);

  /* Create consumer */
  RdKafka::KafkaConsumer *consumer =
      RdKafka::KafkaConsumer::create(conf, errstr);
  if (!consumer) {
    std::cerr << "Failed to create consumer: " << errstr << std::endl;
    exit(1);
  }

  delete conf;

  /* Subscribe to topics */
  RdKafka::ErrorCode err = consumer->subscribe(topics);
  if (err) {
    std::cerr << "Failed to subscribe to " << topics.size()
              << " topics: " << RdKafka::err2str(err) << std::endl;
    exit(1);
  }

  /* Consume messages in batches of \p batch_size */
  while (run) {
    auto msgs = consume_batch(consumer, batch_size, batch_tmout);
    std::cout << "Accumulated " << msgs.size() << " messages:" << std::endl;

    for (auto &msg : msgs) {
      std::string payload(static_cast<const char *>(msg->payload()),
                          msg->len());
      std::cout << " Message in " << msg->topic_name() << " ["
                << msg->partition() << "] at offset " << msg->offset()
                << std::endl
                << "payload: " << payload << std::endl;
      delete msg;
    }
    // break; // TODO REMOVE IN REAL APP
  }

  /* Close and destroy consumer */
  consumer->close();
  delete consumer;

  return 0;
}