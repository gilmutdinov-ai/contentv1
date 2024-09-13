#pragma once

#include "core/UrlFreqStatsLF.h"
#include "ldb/UrlsDaysDb.h"
#include "misc/Config.h"
#include "misc/Log.h"
#include "misc/kafkawrap/KafkaReader.h"
#include <atomic>

namespace contentv1 {

class LoadLoopConfig : public virtual misc::Config {
public:
  LoadLoopConfig();
};

class LoadLoop {

  std::atomic<bool> m_running = true;

  UrlFreqStatsLF::Ptr m_url_freq_stats;
  UrlsDaysDb::Ptr m_urls_days_db;
  misc::KafkaReaderI::Ptr m_kafka_reader;

  Cnt _loopImpl(bool _kafka_dont_block = false);

public:
  LoadLoop(const LoadLoopConfig &cfg, UrlFreqStatsLF::Ptr _url_freq_stats,
           UrlsDaysDb::Ptr _urls_days_db,
           misc::KafkaReaderI::Ptr _kafka_reader);

  ~LoadLoop() { LOG("~LoadLoop() start/finish"); }

  void loop();
  void stop();

  friend class LoadLoopTester;
  friend class Scheduler;
};
} // namespace contentv1