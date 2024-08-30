#pragma once
#include "core/CrawlTargets.h"
#include "core/FetchStats.h"
#include "core/UrlFreqStats.h"
#include "ldb/CrawledDb.h"
#include "ldb/HostDb.h"
#include "ldb/UrlDb.h"
#include "leveldb/db.h"
#include "misc/Config.h"
#include "misc/Log.h"
#include "misc/kafkawrap/KafkaWriter.h"
#include <atomic>

namespace contentv1 {

/*
    Задача - дернуть target_day_crawl в сутки с любым результатом
       имеем target_day_crawl из него выводим
        target_hour_crawl на основе
          - (now - day_end)
          - (target_day_crawl - crawled_today)
        to_enqueue_now_count
          - target_hour_crawl
          - current_hour_finished
          - current_hour_enqueued

        при старте и по таймеру каждый час, по часам ничего не храним

    Самый верхний актор
*/

class EnqueueLoopConfig : public virtual CrawlTargetsConfig {
public:
  inline static const std::string STR_ENQ_LOOP_DB_PATH{
      "sched_enq_loop_db_path"};
  inline static const std::string STR_KAFKA_PUSH_TOPIC{
      "sched_kafka_push_topic"};

private:
  const std::vector<std::string> s_cfg_strs = {STR_ENQ_LOOP_DB_PATH,
                                               STR_KAFKA_PUSH_TOPIC};

public:
  EnqueueLoopConfig();
};

class EnqueueLoop {

  CrawlTargets m_targets;

  std::atomic<bool> m_running;

  //// DB
  UrlFreqStats::Ptr m_url_freq_stats;
  HostDb::Ptr m_host_db;
  UrlDb::Ptr m_url_db;
  CrawledDb::Ptr m_crawled_db;

  //// KAFKA
  misc::KafkaWriterI::Ptr m_kafka_writer;
  std::string m_push_topic;

private:
  Cnt _calcHourTarget();
  Cnt _calcToEnqueueNow();
  void _waitTillEndOfHour();

  Cnt _loopImpl();
  void _checkEnqueueImpl(std::vector<UrlFreq> &_push_urls);

  void _pushToKafka(const std::vector<Url> &_push_urls, Tp _tp);

protected:
  std::vector<EUrlCrawlDecisionPb> _filter(Cnt _top,
                                           std::vector<UrlFreq> &_push_urls);

  /* впринципе надо положить в глобальную очередь
  по конфигу и стате урла, проверить роботс,
  возможно, положить url роботс в out
*/
  std::vector<EUrlCrawlDecisionPb>
  _needCrawlUrls(const std::vector<UrlFreq> &_in_furls,
                 std::vector<UrlFreq> &_out_furls);

public:
  using Ptr = std::shared_ptr<EnqueueLoop>;

  EnqueueLoop(const EnqueueLoopConfig &_config,
              UrlFreqStats::Ptr _url_freq_stats, HostDb::Ptr _host_db,
              UrlDb::Ptr _url_db, CrawledDb::Ptr _crawled_db,
              misc::KafkaWriterI::Ptr _kafka_writer);
  ~EnqueueLoop();

  std::vector<EUrlCrawlDecisionPb> searchTop(Cnt target_urls,
                                             std::vector<UrlFreq> &urls);

  void loop();
  void stop();

  friend class EnqueueLoopTester;
  friend class Scheduler;
};
} // namespace contentv1