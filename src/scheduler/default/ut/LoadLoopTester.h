#pragma once
#include "scheduler/default/LoadLoop.h"

namespace contentv1 {

class LoadLoopTester {

  inline static const std::string s_conf_rel_path{
      "/src/scheduler/default/ut/load_loop_ut.conf"};

  inline static const std::string s_gened_visits_dataset_rel_path{
      "/data/ds/gened_visits_dataset.json"};

  UrlFreqStatsLF::Ptr m_url_freq_stats;
  UrlsDaysDb::Ptr m_urls_days_db;
  misc::KafkaReaderI::Ptr m_kafka_reader;
  std::unique_ptr<LoadLoop> m_load_loop;

public:
  LoadLoopTester();
};

} // namespace contentv1