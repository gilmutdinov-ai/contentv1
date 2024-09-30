#include "UrlsDaysDb.h"

namespace contentv1 {

void UrlsDaysDb::_openLevelDb() {
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, m_db_path.c_str(), &m_db);
  if (!status.ok()) {
    throw std::runtime_error("UrlsDaysDb::_openLevelDb leveldb !ok " +
                             status.ToString());
  }
}

void UrlsDaysDb::_loadSnapshot() {

  // load to local
  UrlsLastDays urls_days_local;
  std::unique_ptr<leveldb::Iterator> it{
      m_db->NewIterator(leveldb::ReadOptions())};
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    urls_days_local.m_urls_days[it->key().ToString()] =
        UrlsLastDays::UrlDaysImpl{it->value().ToString()};
  }
  if (!it->status().ok()) {
    std::cerr << "UrlsDaysDb::_loadSnapshot scan error\n";
    return;
  }
  // write-lock, move to global
  {
    std::unique_lock<std::shared_mutex> write_lock(m_mtx);
    *m_urls_days = std::move(urls_days_local);
  }
}

void UrlsDaysDb::_saveSnapshot() {

  UrlsLastDays urls_days_local;
  // read-lock, copy to local
  {
    std::shared_lock<std::shared_mutex> read_lock(m_mtx);
    urls_days_local = *m_urls_days.get();
  }
  // save to leveldb
  auto end = urls_days_local.m_urls_days.end();
  for (auto it = urls_days_local.m_urls_days.begin(); it != end; ++it) {
    auto s = m_db->Put(leveldb::WriteOptions(), it->first, it->second.dump());
    if (!s.ok()) {
      std::cerr << "UrlsDaysDb::_saveSnapshot could not write leveldb\n";
      return;
    }
  }
}

void UrlsDaysDb::_snapThread() {

  while (m_running.load()) {
    _saveSnapshot();
    sleep(m_snapshot_interval_secs);
  }
  // final snapshot in destructor, after threads finished
}
} // namespace contentv1
