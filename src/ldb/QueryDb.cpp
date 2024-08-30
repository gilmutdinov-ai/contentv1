#include "ldb/QueryDb.h"
#include "misc/Log.h"
#include <filesystem>

namespace contentv1 {

QueryDbConfig::QueryDbConfig() { addStrings(s_cfg_strings); }

QueryDb::QueryDb(const QueryDbConfig &_config) {
  auto _query_db_path = _config[QueryDbConfig::STR_QUERY_DB_PATH].asString();
  auto query_db_qg_path = _query_db_path + "/qg";
  auto query_db_qg_hits_path = _query_db_path + "/hits";

  std::filesystem::create_directories(query_db_qg_path);
  m_qg_db.reset(new misc::LvlDb(query_db_qg_path));

  std::filesystem::create_directories(query_db_qg_hits_path);
  m_qg_hits_db.reset(new misc::LvlDb(query_db_qg_hits_path));
}

bool QueryDb::saveQG(const QueryGroup &_qg, std::string &_err_str) {
  try {
    saveQG(_qg);
  } catch (std::exception &_ex) {
    _err_str = std::string("QueryDb::saveQG ") + _ex.what();
    return false;
  }
  return true;
}

void QueryDb::saveQG(const QueryGroup &_qg) {
  m_qg_db->save(_qg.gid, _qg.dump());
}

bool QueryDb::deleteQG(const GroupId &_gid, std::string &_err_str) {
  try {
    deleteQG(_gid);
  } catch (std::exception &_ex) {
    _err_str = std::string("QueryDb::deleteQG ") + _ex.what();
    return false;
  }
  return true;
}

void QueryDb::deleteQG(const GroupId &_gid) { m_qg_db->del(_gid); }

bool QueryDb::loadQG(const GroupId &_gid, QueryGroup &_qg) {
  std::string dump;
  if (!m_qg_db->load(_gid, dump))
    return false;
  _qg = QueryGroup{dump};
  return true;
}

void QueryDb::getAllQG(std::vector<QueryGroup> &_cgs) {
  m_qg_db->scanFull([&](const std::string &_k, const std::string &_v) {
    _cgs.push_back(QueryGroup{_v});
  });
}

void QueryDb::saveQGHits(const QueryGroupHits &_cg_hits) {
  m_qg_hits_db->save(_cg_hits.gid, _cg_hits.dump());
}

bool QueryDb::saveQGHits(const QueryGroupHits &_cg_hits,
                         std::string &_err_str) {
  try {
    saveQGHits(_cg_hits);
  } catch (std::exception &_ex) {
    _err_str = std::string("QueryDb::saveQGHits ") + _ex.what();
    return false;
  }
  return true;
}

bool QueryDb::loadQGHits(const GroupId &_gid, QueryGroupHits &_cg_hits) {
  std::string dump;
  if (!m_qg_hits_db->load(_gid, dump))
    return false;
  _cg_hits = QueryGroupHits{dump};
  return true;
}

void QueryDb::getAllQGHits(std::vector<QueryGroupHits> &_cg_hits) {
  m_qg_hits_db->scanFull([&](const std::string &_k, const std::string &_v) {
    _cg_hits.push_back(QueryGroupHits{_v});
  });
}

void QueryDb::deleteQGHits(const GroupId &_gid) { m_qg_hits_db->del(_gid); }

bool QueryDb::deleteQGHits(const GroupId &_gid, std::string &_err_str) {
  try {
    deleteQGHits(_gid);
  } catch (std::exception &_ex) {
    _err_str = std::string("QueryDb::deleteQGHits ") + _ex.what();
    return false;
  }
  return true;
}

} // namespace contentv1
