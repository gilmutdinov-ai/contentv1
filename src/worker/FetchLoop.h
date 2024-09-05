#pragma once

#include "core/FetchStats.h"
#include "core/GFQRecord.h"
#include "ldb/PageDb.h"
#include "misc/Config.h"
#include "misc/MergeQueue.h"
#include "misc/Time.h"
#include "misc/Vectors.h"
#include "misc/httpcli/http_client.h"
#include "misc/kafkawrap/KafkaReader.h"
#include "scheduler/SchedulerApi.h"
#include "worker/ReqBatch.h"
#include <atomic>

namespace contentv1 {

/*

FetchLoop
    Читает урлы из кафки и пытается скачать, отстукивая в шедулер
*/

struct FetchLoopConfig : public virtual misc::Config {

  // mimic yabot
  std::string user_agent =
      "Mozilla/5.0 (compatible; YandexBot/3.0; +http://yandex.com/bots)";
  long request_timeout_secs = 3;
  long select_interval_usecs = 50000;
  long merge_interval_secs = 1;
  long max_requests_in_batch = 128;
  long kafka_read_batch_size = 128;

  inline static const std::string STR_USER_AGENT{"fetch_user_agent"};
  inline static const std::string INT_REQUEST_TIMEOUT_SECS{
      "fetch_request_timeout_secs"};
  inline static const std::string INT_SELECT_INTERVAL_USECS{
      "fetch_select_interval_usecs"};
  inline static const std::string INT_MERGE_INTERVAL_SECS{
      "fetch_merge_interval_secs"};
  inline static const std::string INT_MAX_REQUESTS_IN_BATCH{
      "fetch_max_requests_in_batch"};
  inline static const std::string INT_KAFKA_READ_BATCH_SIZE{
      "fetch_kafka_read_batch_size"};

private:
  const std::vector<std::string> c_strings_opt{STR_USER_AGENT};
  const std::vector<std::string> c_ints_opt{
      INT_REQUEST_TIMEOUT_SECS, INT_SELECT_INTERVAL_USECS,
      INT_MERGE_INTERVAL_SECS, INT_MAX_REQUESTS_IN_BATCH,
      INT_KAFKA_READ_BATCH_SIZE};

public:
  // clang? https://stackoverflow.com/a/44693603
  FetchLoopConfig() noexcept;
  void onParsed();
};

// 1 request / request batch to push
using FetchLoopMergeQueue = misc::MergeQueue<GFQRecord, ReqBatch>;

class FetchLoop : public FetchLoopMergeQueue {
public:
private:
  std::atomic<bool> m_running;
  misc::KafkaReaderI::Ptr m_kafka_reader;
  SchedulerApi::Ptr m_scheduler_api;

  using HttpClient = misc::HttpClientA<GFQRecord>;
  using HttpClientPtr = std::shared_ptr<HttpClient>;
  HttpClientPtr m_client;

  const long m_kafka_read_batch_size;
  const long m_max_requests_in_batch;

  IPageDb::Ptr m_page_db;

  std::atomic<long> m_pushed_to_queue{0};
  std::atomic<long> m_success_count{0};
  std::atomic<long> m_fail_count{0};
  std::atomic<long> m_robots_count{0};
  std::atomic<long> m_robots_fail_count{0};

public:
  using Ptr = std::shared_ptr<FetchLoop>;

  FetchLoop(misc::KafkaReaderI::Ptr _kafka_reader,
            SchedulerApi::Ptr _scheduler_api, IPageDb::Ptr _page_db,
            const FetchLoopConfig &_config = FetchLoopConfig{});

  ~FetchLoop();

  void stop();
  void loop();

  FetchStats getStats() const;

private:
  void _loopImpl();
  void _onResponse(HttpClient::JobInfo _ji);

  // MERGE QUEUE
  std::unique_ptr<ReqBatch> m_req_batch;

  void accEvent(ReqBatch &_batch, const GFQRecord &_req) override;
  ReqBatch *copyGlobalObj() override;
  void merge(ReqBatch &_a, const ReqBatch &_b) override;
  void switchGlobalObj(ReqBatch *_a) override;

  //// called from switchGlobalObj
  void _sendToCurl(ReqBatch *_a) {

    //    auto now_ts = misc::to_int64(misc::get_now());
    while (!_a->empty()) {
      std::vector<GFQRecord> try_urls;
      auto batch_size = _a->getReqs(m_max_requests_in_batch, try_urls);
      *_a = _a->slice(batch_size);

      /// CALL_SCHEDULER_API_AND_PUSH_NOT_ALLOWED_BACK_TO_QUEUE
      std::vector<GFQRecord> allowed_urls;
      m_scheduler_api->tryFetchUrls(try_urls, allowed_urls);
      /*
            std::cout << "allowed_urls: \n";
            print(allowed_urls);
      */
      for (size_t i = 0; i < allowed_urls.size(); ++i)
        m_client->call(allowed_urls[i], allowed_urls[i].url);

      std::sort(try_urls.begin(), try_urls.end());
      std::sort(allowed_urls.begin(), allowed_urls.end());
      std::vector<GFQRecord> disallowed_urls;
      misc::push_vector_minus(try_urls, allowed_urls, disallowed_urls);
      for (size_t i = 0; i < disallowed_urls.size(); ++i)
        FetchLoopMergeQueue::push(disallowed_urls[i]);
    }
    m_client->kick();
  }
  friend class FetchLoopTester;
  friend class Worker;
};

} // namespace contentv1
