#pragma once

#include "core/Types.h"
#include "db/IPageDb.h"
#include "misc/Config.h"
#include "misc/LvlDb.h"
#include <memory>

namespace contentv1 {

class PageDbConfig : virtual public misc::Config {
public:
  inline static const std::string STR_PAGE_DB_PATH{"page_db_path"};

private:
  const std::vector<std::string> s_cfg_strings = {STR_PAGE_DB_PATH};

public:
  PageDbConfig();
};

class PageDb final : public IPageDb {
  misc::LvlDb m_db;

public:
  PageDb(const PageDbConfig &_config);
  virtual ~PageDb() = default;
  void save(const Url &_url, const std::string &_content) override;
  bool load(const Url &_url, std::string &_content) override;

  // using ScanPageCb = std::function<void(const Url &, const std::string &)>;
  void scanFull(ScanPageCb _cb) override;
};

} // namespace contentv1