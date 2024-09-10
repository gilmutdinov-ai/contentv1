#pragma once
#include "core/FetchStats.h"
#include "core/UrlFreqStatsLF.h"
#include "ldb/CrawledDb.h"
#include "ldb/HostDb.h"
#include "ldb/UrlDb.h"
#include "ldb/UrlsDaysDb.h"
#include "misc/Config.h"
#include "scheduler/IScheduler.h"
#include "scheduler/default/EnqueueLoop.h"
#include "scheduler/default/LoadLoop.h"
#include <thread>

/*

Шедулер закидывает урлы в очередь на скачку (kafka:global_fetch_queue) так,
чтобы выполнить часовые, суточные квоты

Воркер ходит в апи (пачкой) и перед самой скачкой (push to curl evloop)
  - опрашивает шедулер можно ли прямо сейчас качать
    - не положить хосты (max host {parallel} crawl)
    - не положить себя (max system crawl)
  - если нельзя, закидывает в начало своей очереди

Скачивание urlов повторно:
  - в UrlDbPolicy
*/

namespace contentv1 {

// config
class SchedulerConfig : public virtual LoadLoopConfig,
                        public virtual EnqueueLoopConfig,
                        public virtual HostDbConfig,
                        public virtual UrlDbConfig,
                        public virtual UrlsDaysDbConfig,
                        public virtual CrawledDbConfig {
public:
  inline static const std::string STR_ARRAY_KAFKA_BROKERS{
      "sched_kafka_brokers"};
  inline static const std::string STR_KAFKA_INPUT_STREAM_VISITS_TOPIC{
      "sched_kafka_input_stream_visits_topic"};
  inline static const std::string INT_URL_FREQ_STATS_LF_MERGE_INTERVAL_SECS{
      "sched_url_freq_stats_lf_merge_interval_secs"};

private:
  const std::vector<std::string> s_cfg_arrays = {STR_ARRAY_KAFKA_BROKERS};
  const std::vector<std::string> s_cfg_strings = {
      STR_KAFKA_INPUT_STREAM_VISITS_TOPIC};
  const std::vector<std::string> s_cfg_ints = {
      INT_URL_FREQ_STATS_LF_MERGE_INTERVAL_SECS};

public:
  SchedulerConfig();
  virtual ~SchedulerConfig() = default;
  //  SchedulerConfig(const std::string &fn);
};

class Scheduler : public IScheduler {
  // persistent, concurrently accessed
  HostDb::Ptr m_host_db;
  UrlDb::Ptr m_url_db;
  UrlsDaysDb::Ptr m_urls_days_db;
  CrawledDb::Ptr m_crawled_db;
  // kafka
  misc::KafkaReaderI::Ptr m_kafka_reader;
  misc::KafkaWriterI::Ptr m_kafka_writer;

  // stats
  UrlFreqStatsLF::Ptr m_url_freq_stats;

  // threads
  std::unique_ptr<LoadLoop> m_load_loop;
  std::unique_ptr<EnqueueLoop> m_enqueue_loop;

  std::unique_ptr<std::thread> m_load_th;
  std::unique_ptr<std::thread> m_enqueue_th;

  static Cnt _fillUrlFreqStats(UrlsDaysDb::Ptr _urls_days_db,
                               UrlFreqStatsLF::Ptr _url_freq_stats);

  Cnt m_loaded_uts_count{0};

  // fetch stats
  std::atomic<long> m_success_count{0};
  std::atomic<long> m_fail_count{0};
  std::atomic<long> m_robots_count{0};
  std::atomic<long> m_robots_fail_count{0};

  void _incStats(const std::vector<FetchResult> &_results);

  bool m_dry_run;

public:
  using Ptr = std::shared_ptr<Scheduler>;

  Scheduler(const SchedulerConfig &_config, bool _dry_run = false,
            bool _in_bazel_test = false, bool _mock_kafka = false);
  virtual ~Scheduler();

  FetchStats getStats() const;

  void start();
  void stop();
  void join();

  // API IMPLEMENTATION
  //
public:
  void tryFetchUrls(const std::vector<GFQRecord> &_try_urls,
                    std::vector<GFQRecord> &_allowed_urls) override;

  void onFetched(const std::vector<FetchResult> &_results) override;

  //
  friend class SchedulerTester;
  friend class SchedulerServerTester;
};
} // namespace contentv1
