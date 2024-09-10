#include "worker/FetchLoop.h"
#include "kafkawrap/GFQReader.h"
#include "misc/Log.h"
#include "src/proto/CrawlAttempts.pb.h"

namespace contentv1 {

FetchLoopConfig::FetchLoopConfig() noexcept {
  addStringsOpt(c_strings_opt);
  addIntsOpt(c_ints_opt);
  addOnParsedCb(std::bind(&FetchLoopConfig::onParsed, this));
};

void FetchLoopConfig::onParsed() {

  if ((*this)[FetchLoopConfig::STR_USER_AGENT].isString())
    user_agent = (*this)[FetchLoopConfig::STR_USER_AGENT].asString();
  if ((*this)[FetchLoopConfig::INT_REQUEST_TIMEOUT_SECS].isInt())
    request_timeout_secs =
        (*this)[FetchLoopConfig::INT_REQUEST_TIMEOUT_SECS].asInt();
  if ((*this)[FetchLoopConfig::INT_SELECT_INTERVAL_USECS].isInt())
    select_interval_usecs =
        (*this)[FetchLoopConfig::INT_SELECT_INTERVAL_USECS].asInt();
  if ((*this)[FetchLoopConfig::INT_MERGE_INTERVAL_SECS].isInt())
    merge_interval_secs =
        (*this)[FetchLoopConfig::INT_MERGE_INTERVAL_SECS].asInt();
  if ((*this)[FetchLoopConfig::INT_MAX_REQUESTS_IN_BATCH].isInt())
    max_requests_in_batch =
        (*this)[FetchLoopConfig::INT_MAX_REQUESTS_IN_BATCH].asInt();
  if ((*this)[FetchLoopConfig::INT_KAFKA_READ_BATCH_SIZE].isInt())
    kafka_read_batch_size =
        (*this)[FetchLoopConfig::INT_KAFKA_READ_BATCH_SIZE].asInt();
}

FetchLoop::FetchLoop(misc::KafkaReaderI::Ptr _kafka_reader,
                     IScheduler::Ptr _scheduler_api, IPageDb::Ptr _page_db,
                     const FetchLoopConfig &_config)
    : m_running(true), FetchLoopMergeQueue(_config.merge_interval_secs),
      m_req_batch(new ReqBatch), m_kafka_reader(_kafka_reader),
      m_scheduler_api(_scheduler_api),
      m_client(new HttpClient{
          std::bind(&FetchLoop::_onResponse, this, std::placeholders::_1),
          HttpClient::HttpSettings{_config.user_agent,
                                   _config.request_timeout_secs,
                                   _config.select_interval_usecs}}),
      m_kafka_read_batch_size{_config.kafka_read_batch_size},
      m_max_requests_in_batch{_config.max_requests_in_batch},
      m_page_db{_page_db} {
  FetchLoopMergeQueue::start();
}

FetchLoop::~FetchLoop() {
  LOG("~FetchLoop() start");
  stop();
  FetchLoopMergeQueue::stop();
  FetchLoopMergeQueue::join();
  LOG("~FetchLoop() finish");
}

void FetchLoop::stop() { m_running.store(false); }

void FetchLoop::loop() {
  while (m_running.load()) {
    if (FetchLoopMergeQueue::size_approx() > m_kafka_read_batch_size) {
      sleep(1);
      continue;
    }
    _loopImpl();
  }
  _loopImpl();
}

void FetchLoop::_loopImpl() {

  GFQReader::read(
      m_kafka_reader,
      [&](const GFQRecord &_gfq) {
        // std::cout << "pushing to queue: " << _gfq.url << std::endl;
        //        LOG("pushing to queue: " << _gfq.url);
        ++m_pushed_to_queue;
        FetchLoopMergeQueue::push(_gfq);
        m_client->kick();
      },
      m_kafka_read_batch_size);
  m_client->kick();
}

void FetchLoop::_onResponse(HttpClient::JobInfo _ji) {

  // stats
  if (_ji.success) {
    if (_ji.userdata.isRobots())
      ++m_robots_count;
    else
      ++m_success_count;
  } else { // fail
    if (_ji.userdata.isRobots())
      ++m_robots_fail_count;
    else
      ++m_fail_count;
  }

  // std::cout << "FetchLoop::_onResponse url:" << _ji.userdata.url <<
  // std::endl;
  //   std::cout << "FetchLoop::_onResponse ts:" << _ji.userdata.attempt_ts
  //             << std::endl;
  // std::cout << "FetchLoop::_onResponse success: " << _ji.success <<
  // std::endl;

  CrawlAttemptsPb::AttemptStatus status = CrawlAttemptsPb::ATTEMPT_STATUS_OK;
  std::string error_str;
  std::string content;

  if (!_ji.success) {
#warning FIX_ADD_4xx_5xx
    status = CrawlAttemptsPb::ATTEMPT_STATUS_CURL_ERR;
    error_str = _ji.curlerror;
  }

  if (_ji.success && _ji.userdata.isRobots()) {
    content = _ji.resp;
  }

  FetchResult res{_ji.userdata, status, error_str, content};
  std::vector<FetchResult> results{res}; // buffer later
  m_scheduler_api->onFetched(results);

  if (_ji.success && !_ji.userdata.isRobots()) {
    m_page_db->save(_ji.userdata.url, _ji.resp);
  }
}

FetchStats FetchLoop::getStats() const {
  return FetchStats{m_pushed_to_queue.load(), m_success_count.load(),
                    m_fail_count.load(), m_robots_count.load(),
                    m_robots_fail_count.load()};
}

///// MERGE QUEUE
void FetchLoop::accEvent(ReqBatch &_batch, const GFQRecord &_req) {

  m_req_batch->acc(_req);
  m_client->kick();
}

ReqBatch *FetchLoop::copyGlobalObj() {
  m_client->kick();
  return new ReqBatch{*m_req_batch};
}

void FetchLoop::merge(ReqBatch &_a, const ReqBatch &_b) {
  m_client->kick();
  _a.merge(_b);
}

void FetchLoop::switchGlobalObj(ReqBatch *_a) {
  // нарезать на реальные запросы и послать в curl
  _sendToCurl(_a);
  m_req_batch.reset(_a);
  m_client->kick();
}

} // namespace contentv1
