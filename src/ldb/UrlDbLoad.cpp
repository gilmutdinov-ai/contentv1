#include "UrlDb.h"

namespace contentv1 {

/*
Call only in UrlDb constructor,
before clients or snap thread can access maps
*/

void UrlDb::_loadData(const std::string &_url_db_attempts_path) {
  _loadCrawlAttempts(_url_db_attempts_path);
}

void UrlDb::_loadCrawlAttempts(const std::string &_url_db_attempts_path) {

  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status =
      leveldb::DB::Open(options, _url_db_attempts_path.c_str(), &m_attempts_db);
  if (!status.ok())
    throw std::runtime_error("UrlDb::loadCrawlAttempts leveldb !ok");
  std::unique_ptr<leveldb::Iterator> it{
      m_attempts_db->NewIterator(leveldb::ReadOptions())};
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    m_url_crawl_attempts[it->key().ToString()] =
        CrawlAttempts{it->value().ToString()};
  }
  if (!it->status().ok()) {
    std::cerr << "UrlDb::loadCrawlAttempts scan error\n";
  }
}

} // namespace contentv1