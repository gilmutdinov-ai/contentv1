#pragma once
#include "misc/kafkawrap/KafkaReader.h"
#include "scheduler/SchedulerApi.h"
#include "worker/FetchLoop.h"

namespace contentv1 {

class FetchLoopTester {

  inline static const std::string s_gened_visits_dataset_rel_path{
      "/data/ds/gened_visits_dataset.json"};

  misc::KafkaReader::Ptr m_kafka_reader;
  SchedulerApi::Ptr m_scheduler_api;
  IPageDb::Ptr m_page_db;
  FetchLoop::Ptr m_fetch_loop;

  static std::vector<std::string> genGFQDataset(bool _under_bazel_test,
                                                Ts _attempt_ts);

public:
  FetchLoopTester(bool _under_bazel_test = true);
};

} // namespace contentv1