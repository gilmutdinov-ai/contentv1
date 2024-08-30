#include "worker/Worker.h"
#include "misc/Js.h"
#include "misc/Log.h"
#include "misc/kafkawrap/KafkaReaderMock.h"

namespace contentv1 {

Worker::Worker(IPageDb::Ptr _page_db, SchedulerApi::Ptr _scheduler_api,
               const WorkerConfig &_config, bool _is_dry_run)
    : m_is_dry_run(_is_dry_run), m_page_db{_page_db},
      m_scheduler_api(_scheduler_api) {

  if (_is_dry_run) {
    // sim
    m_kafka_reader.reset(new misc::KafkaReaderMock{
        _config[WorkerConfig::STR_FETCH_TOPIC].asString()});
  } else {
    m_kafka_reader.reset(new misc::KafkaReader{
        _config[WorkerConfig::STR_FETCH_TOPIC].asString(),
        misc::get_str_array_from_json(_config[WorkerConfig::ARR_KAFKA_BROKERS]),
        _config[WorkerConfig::STR_KAFKA_GROUP_ID].asString()});
  }

  m_fetch_loop.reset(new FetchLoop(m_kafka_reader, _scheduler_api, m_page_db));

  if (_is_dry_run) {
    LOG("Calling m_fetch_loop->_loopImpl()..");
    m_fetch_loop->_loopImpl();
    LOG("Calling m_fetch_loop->_loopImpl()..DONE");
  }
}

Worker::~Worker() {
  LOG("~Worker() start");
  stop();
  if (!m_is_dry_run)
    join();
  LOG("~Worker() finish");
}

FetchStats Worker::getStats() const { return m_fetch_loop->getStats(); }

void Worker::start() {
  m_fetch_th.reset(new std::thread(std::bind(&Worker::loop, this)));
}

void Worker::stop() {
  if (m_fetch_loop)
    m_fetch_loop->stop();
}

void Worker::join() {
  if (m_fetch_loop)
    m_fetch_loop->join();
}

void Worker::loop() { m_fetch_loop->loop(); }

// stop
//  if (m_query_loop)
//   m_query_loop->stop();

// join
// if (m_query_loop)
//   m_query_th->join();

} // namespace contentv1