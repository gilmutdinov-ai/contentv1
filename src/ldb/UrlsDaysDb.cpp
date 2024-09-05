#include "UrlsDaysDb.h"
#include <filesystem>

namespace contentv1 {

UrlsDaysDbConfig::UrlsDaysDbConfig() {
  addStrings(s_cfg_strs);
  addInts(s_cfg_ints);
}

UrlsDaysDb::UrlsDaysDb(const UrlsDaysDbConfig &_config)
    : m_db_path{_config[UrlsDaysDbConfig::STR_URLS_DAYS_DB_PATH].asString()},
      m_snapshot_interval_secs{
          _config[UrlsDaysDbConfig::INT_SNAPSHOT_INTERVAL_SECS].asInt()},
      m_merge_interval_secs{
          _config[UrlsDaysDbConfig::INT_MERGE_INTERVAL_SECS].asInt()},
      m_running(true) {
  // after configs parsed
  _config.validate();

  std::filesystem::create_directories(m_db_path);

  _openLevelDb();
  m_urls_days.reset(new UrlsLastDays{});
  _loadSnapshot();
  m_merge_th.reset(new std::thread(std::bind(&UrlsDaysDb::_mergeThread, this)));
  m_snap_th.reset(new std::thread(std::bind(&UrlsDaysDb::_snapThread, this)));
}

UrlsDaysDb::~UrlsDaysDb() {
  m_running.store(false);
  m_merge_th->join();
  m_snap_th->join();
  _saveSnapshot();
  delete m_db;
}

void UrlsDaysDb::saveVisit(const UrlVisit &_url_visit) {
  UrlTpStat utp{_url_visit};
  saveVisitsPack(utp);
}

void UrlsDaysDb::saveVisitsPack(const UrlTpStat &_utp) { m_q.enqueue(_utp); }

void UrlsDaysDb::waitMerged() {
  std::unique_lock<std::mutex> lck(m_merge_mtx);
  m_merge_cv.wait(lck, [&] { return m_merge_finished; });
}

void UrlsDaysDb::_mergeThread() {

  while (m_running.load()) {
    _mergeEventsFromQueue();
    sleep(m_merge_interval_secs);
  }
  _mergeEventsFromQueue();
}

void UrlsDaysDb::_setMergeStarted() {
  std::lock_guard<std::mutex> lck(m_merge_mtx);
  m_merge_finished = false;
}

void UrlsDaysDb::_setMergeFinished() {
  {
    std::lock_guard<std::mutex> lck(m_merge_mtx);
    m_merge_finished = true;
  }
  m_merge_cv.notify_all();
}

void UrlsDaysDb::_mergeEventsFromQueue() {

  _setMergeStarted();

  UrlsLastDays upd_urls_days_local;

  UrlTpStat utp;
  while (m_q.try_dequeue(utp)) {
    upd_urls_days_local.add(utp);
  }

  if (upd_urls_days_local.empty()) {
    //_setMergeFinished();
    return;
  }

  // read-lock, copy from main
  UrlsLastDays urls_days_global_copy;
  {
    std::shared_lock<std::shared_mutex> read_lock(m_mtx);
    urls_days_global_copy = *m_urls_days.get();
  }
  // merge locally
  urls_days_global_copy.merge(upd_urls_days_local, misc::get_now());
  // write lock, update global
  {
    std::unique_lock<std::shared_mutex> write_lock(m_mtx);
    *m_urls_days = std::move(urls_days_global_copy);
  }

  _setMergeFinished();
}

Cnt UrlsDaysDb::size() {

  std::shared_lock<std::shared_mutex> read_lock(m_mtx);
  return m_urls_days->size();
}

void UrlsDaysDb::print() {
  std::shared_lock<std::shared_mutex> read_lock(m_mtx);
  return m_urls_days->print();
}

bool UrlsDaysDb::getUTSV(const Url &_url, std::vector<UrlTpStat> &_utsv) {
  std::shared_lock<std::shared_mutex> read_lock(m_mtx);
  return m_urls_days->getUTSV(_url, _utsv);
}

void UrlsDaysDb::iterateAll(
    std::function<void(const Url &, const std::vector<UrlTpStat> &)> _cb) {

  std::shared_lock<std::shared_mutex> read_lock(m_mtx);
  m_urls_days->iterateAll(_cb);
}

bool operator==(const UrlsDaysDb &a, const UrlsDaysDb &b) {

  return a.m_urls_days == b.m_urls_days;
}

} // namespace contentv1
