#pragma once
#include "worker/FetchLoop.h"
#include "worker/QueryLoop.h"
#include <memory>
#include <thread>

// #include "scheduler/SchedulerApi.h"

namespace contentv1 {

/*
const std::string &_topic,
              const std::vector<std::string> &_brokers,
              const std::string &_group_id = "test_kafka"
*/

class WorkerConfig : public virtual FetchLoopConfig {
public:
  inline static const std::string STR_FETCH_TOPIC{"fetch_kafka_read_topic"};
  inline static const std::string ARR_KAFKA_BROKERS{"fetch_kafka_brokers"};
  inline static const std::string STR_KAFKA_GROUP_ID{"fetch_kafka_group_id"};

  WorkerConfig() {
    addArrays({ARR_KAFKA_BROKERS});
    addStrings({STR_FETCH_TOPIC, STR_KAFKA_GROUP_ID});
  }
};

class Worker {
  // std::unique_ptr<QueryLoop> m_query_loop;
  // std::unique_ptr<std::thread> m_query_th;

  SchedulerApi::Ptr m_scheduler_api;
  misc::KafkaReader::Ptr m_kafka_reader;
  IPageDb::Ptr m_page_db;
  FetchLoop::Ptr m_fetch_loop;
  std::unique_ptr<std::thread> m_fetch_th;

  bool m_is_dry_run;

public:
  using Ptr = std::shared_ptr<Worker>;

  Worker(IPageDb::Ptr _page_db, SchedulerApi::Ptr _scheduler_api,
         const WorkerConfig &_config, bool _is_dry_run = false);
  ~Worker();

  FetchStats getStats() const;

  void start();
  void stop();
  void join();

private:
  void loop();
  void _loopImpl();
};
} // namespace contentv1