#include "UrlDb.h"

namespace contentv1 {

void UrlDb::_snapThread() {
  //   std::unordered_map<Url, CrawlAttempts> m_url_crawl_attempts;

  while (m_running.load()) {
    _snapData();
    sleep(m_snapshot_interval_sec);
  }
  _snapData();
}

void UrlDb::_snapData() {
  std::unordered_map<Url, CrawlAttempts> url_crawl_attempts_local_copy;
  {
    std::lock_guard<std::mutex> lock(m_mtx);
    url_crawl_attempts_local_copy = m_url_crawl_attempts;
  }

  auto it = url_crawl_attempts_local_copy.begin();
  auto end = url_crawl_attempts_local_copy.end();
  while (it != end) {
    auto s = m_attempts_db->Put(leveldb::WriteOptions(), it->first,
                                it->second.dump());
    if (!s.ok()) {
      std::cerr << "UrlDb::_snapData could not write leveldb\n";
    }
    ++it;
  }
}

} // namespace contentv1
