#include "scheduler/LoadLoop.h"
#include "kafkawrap/UrlVisitsReader.h"

namespace contentv1 {

LoadLoopConfig::LoadLoopConfig() {
  // add needed fields
}

LoadLoop::LoadLoop(const LoadLoopConfig &_cfg,
                   UrlFreqStatsLF::Ptr _url_freq_stats,
                   UrlsDaysDb::Ptr _urls_days_db,
                   misc::KafkaReaderI::Ptr _kafka_reader)
    : m_running(true), m_url_freq_stats(_url_freq_stats),
      m_urls_days_db(_urls_days_db), m_kafka_reader(_kafka_reader) {}

void LoadLoop::loop() {

  while (m_running.load()) {
    _loopImpl();
  }
}

Cnt LoadLoop::_loopImpl() {
  Cnt loaded_cnt = 0;
  UrlVisitsReader::read(m_kafka_reader, [&](const UrlVisit &_uv) {
    auto now = misc::get_now();
    m_url_freq_stats->insert(_uv, now);
    m_urls_days_db->saveVisit(_uv);
    ++loaded_cnt;

    if (!m_running.load())
      m_kafka_reader->stop();
  });
  return loaded_cnt;
}

void LoadLoop::stop() { m_running.store(false); }

} // namespace contentv1
