#include "ldb/PageDb.h"
#include <filesystem>

namespace contentv1 {

PageDbConfig::PageDbConfig() { addStrings(s_cfg_strings); }

PageDb::PageDb(const PageDbConfig &_config)
    : m_db{_config[PageDbConfig::STR_PAGE_DB_PATH].as<std::string>()} {}

void PageDb::save(const Url &_url, const std::string &_content) {
  m_db.save(_url, _content);
}

bool PageDb::load(const Url &_url, std::string &_content) {
  return m_db.load(_url, _content);
}

void PageDb::scanFull(ScanPageCb _cb) { m_db.scanFull(_cb); }

} // namespace contentv1
