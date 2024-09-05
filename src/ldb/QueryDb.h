#pragma once

#include "core/QueryGroup.h"
#include "core/Types.h"
#include "misc/Config.h"
#include "misc/LvlDb.h"
#include <memory>

namespace contentv1 {

class QueryDbConfig : virtual public misc::Config {
public:
  inline static const std::string STR_QUERY_DB_PATH{"query_db_path"};

private:
  const std::vector<std::string> s_cfg_strings = {STR_QUERY_DB_PATH};

public:
  QueryDbConfig();
  virtual ~QueryDbConfig() = default;
};

class QueryDb {
  std::unique_ptr<misc::LvlDb> m_qg_db;
  std::unique_ptr<misc::LvlDb> m_qg_hits_db;

public:
  using Ptr = std::shared_ptr<QueryDb>;

  QueryDb(const QueryDbConfig &_config);

  bool saveQG(const QueryGroup &_qg, std::string &_err_str);
  void saveQG(const QueryGroup &_qg);

  bool loadQG(const GroupId &_gid, QueryGroup &_qg);
  bool deleteQG(const GroupId &_gid, std::string &_err_str);
  void deleteQG(const GroupId &_gid);

  void getAllQG(std::vector<QueryGroup> &_cgs);

  void saveQGHits(const QueryGroupHits &_cg_hits);
  bool saveQGHits(const QueryGroupHits &_cg_hits, std::string &_err_str);
  bool loadQGHits(const GroupId &_gid, QueryGroupHits &_cg_hits);
  void deleteQGHits(const GroupId &_gid);
  bool deleteQGHits(const GroupId &_gid, std::string &_err_str);

  void getAllQGHits(std::vector<QueryGroupHits> &_cg_hits);
};

} // namespace contentv1