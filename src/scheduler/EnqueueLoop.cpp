#include "scheduler/EnqueueLoop.h"
#include "core/GFQRecord.h"
#include "misc/Log.h"

namespace contentv1 {

void EnqueueLoop::loop() {
  while (m_running.load()) {
    _loopImpl();
    _waitTillEndOfHour();
  }
}

static void to_urls_vec(const std::vector<UrlFreq> &_urls_freq,
                        std::vector<Url> &_urls) {
  _urls.reserve(_urls_freq.size());
  for (size_t i = 0; i < _urls_freq.size(); ++i)
    _urls.push_back(_urls_freq[i].first);
}

Cnt EnqueueLoop::_loopImpl() {

  LOG("EnqueueLoop::_loopImpl");
  std::vector<UrlFreq> push_urls_freq;
  _checkEnqueueImpl(push_urls_freq);

  auto attempt_tp{misc::get_now()};

  std::vector<Url> push_urls;
  to_urls_vec(push_urls_freq, push_urls);

  LOG("EnqueueLoop::_loopImpl pushing urls: " << push_urls.size());

  _pushToKafka(push_urls, attempt_tp);
  m_crawled_db->saveResult(CrawlAttemptsPb::ATTEMPT_STATUS_ENQUEUED,
                           push_urls.size());
  m_url_db->setEnqueued(push_urls, attempt_tp);

  return push_urls.size();
}

void EnqueueLoop::_checkEnqueueImpl(std::vector<UrlFreq> &_push_urls) {

  Cnt push_target = _calcToEnqueueNow();
  if (push_target <= 0) {
    LOG("EnqueueLoop::_checkEnqueue no need to enqueue now");
    return;
  }

  std::vector<EUrlCrawlDecisionPb> decisions =
      searchTop(push_target, _push_urls);
}

void EnqueueLoop::_waitTillEndOfHour() {

  std::this_thread::sleep_for(misc::get_hour_end() - misc::get_now());
}

Cnt EnqueueLoop::_calcHourTarget() {

  // auto till_day_end = misc::get_now() - misc::get_day_end();
  Cnt crawled_today = m_crawled_db->getFinishedToday();
  LOG("crawled_today: " << crawled_today);
  auto left_crawl_today = m_targets.target_day_crawl - crawled_today;
  LOG("left_crawl_today: " << left_crawl_today);
  auto hours_till_midnight = misc::get_hours_till_midnight();
  LOG("hours_till_midnight: " << hours_till_midnight);
  if (hours_till_midnight == 0)
    return left_crawl_today;
  return left_crawl_today / hours_till_midnight;
}

Cnt EnqueueLoop::_calcToEnqueueNow() {

  auto target_current_hour = _calcHourTarget();
  LOG("target_current_hour: " << target_current_hour);
  auto in_progress_now = m_crawled_db->getInProgress();
  LOG("in_progress_now: " << in_progress_now);

  if (in_progress_now < target_current_hour)
    return target_current_hour - in_progress_now;
  return 0;
}

void EnqueueLoop::_pushToKafka(const std::vector<Url> &_push_urls, Tp _tp) {

  for (size_t i = 0; i < _push_urls.size(); ++i) {
    GFQRecord gfq{_push_urls[i], misc::to_int64(_tp)};
    m_kafka_writer->write(m_push_topic, gfq.dump());
  }
}

} // namespace contentv1