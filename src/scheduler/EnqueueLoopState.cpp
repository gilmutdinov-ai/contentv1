#include "EnqueueLoop.h"

namespace contentv1 {

EnqueueLoopConfig::EnqueueLoopConfig() { addStrings(s_cfg_strs); }

EnqueueLoop::EnqueueLoop(const EnqueueLoopConfig &_config,
                         UrlFreqStats::Ptr _url_freq_stats,
                         HostDb::Ptr _host_db, UrlDb::Ptr _url_db,
                         CrawledDb::Ptr _crawled_db,
                         misc::KafkaWriterI::Ptr _kafka_writer)
    : m_targets(_config), m_running(true), m_url_freq_stats(_url_freq_stats),
      m_host_db(_host_db), m_url_db(_url_db), m_crawled_db(_crawled_db),
      m_kafka_writer(_kafka_writer),
      m_push_topic{
          _config[EnqueueLoopConfig::STR_KAFKA_PUSH_TOPIC].asString()} {}

EnqueueLoop::~EnqueueLoop() {
  LOG("~LoadLoop() start");
  stop();
  LOG("~LoadLoop() finish");
}

void EnqueueLoop::stop() { m_running.store(false); }

} // namespace contentv1
