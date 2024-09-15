#include "UrlDb.h"
#include "misc/Time.h"
#include <filesystem>

namespace contentv1 {

UrlDbConfig::UrlDbConfig() {
  addStrings(s_cfg_strings);
  addInts(s_cfg_ints);
}

UrlDb::UrlDb(const UrlDbConfig &_config)
    : m_snapshot_interval_sec{
          _config[UrlDbConfig::INT_URL_DB_SNAPSHOT_INTERVAL_SEC].as<int>()} {

  auto db_path{_config[UrlDbConfig::STR_URL_DB_PATH].as<std::string>()};
  std::filesystem::create_directories(db_path);

  _loadData(db_path);
  m_running.store(true);
  m_snap_th.reset(new std::thread(std::bind(&UrlDb::_snapThread, this)));
}

UrlDb::~UrlDb() {
  m_running.store(false);
  m_snap_th->join();
  delete m_attempts_db;
  //  delete m_pers_stats_db;
}

bool UrlDb::getAttempts(const Url &_url, CrawlAttempts &_ca) {
  std::lock_guard<std::mutex> lock(m_mtx);
  auto it = m_url_crawl_attempts.find(_url);
  if (it == m_url_crawl_attempts.end())
    return false;
  _ca = it->second;
  return true;
}

Cnt UrlDb::size() {
  std::lock_guard<std::mutex> lock(m_mtx);
  return m_url_crawl_attempts.size();
}

} // namespace contentv1