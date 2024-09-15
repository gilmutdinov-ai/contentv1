#pragma once

#include "cassandra.h"
#include "core/Types.h"
#include "db/IPageDb.h"
#include "misc/Config.h"
#include "misc/LvlDb.h"
#include <memory>

/*

Реализация PageDb на ScyllaDb

Как потом ускорять фулсканы:
https://www.scylladb.com/2017/03/28/parallel-efficient-full-table-scan-scylla/

*/

namespace contentv1 {

class PageDbScyllaConfig : virtual public misc::Config {
public:
  inline static const std::string STR_SCYLLA_HOSTS{"scylla_hosts"};

private:
  const std::vector<std::string> s_cfg_arrays = {STR_SCYLLA_HOSTS};

public:
  PageDbScyllaConfig();
};

class PageDbScylla final : public IPageDb {

  inline static constexpr const char *s_truncate_table_query =
      "TRUNCATE contentv1.web_pages";

public:
  PageDbScylla(const PageDbScyllaConfig &_config);
  virtual ~PageDbScylla();
  void save(const Url &_url, const std::string &_content) override;
  bool load(const Url &_url, std::string &_content) override;

  // using ScanPageCb = std::function<void(const Url &, const std::string &)>;
  void scanFull(ScanPageCb _cb) override;

  void deleteAll();

private:
  CassFuture *m_connect_future = NULL;
  CassCluster *m_cluster;
  CassSession *m_session;

private:
  static std::string getError(CassFuture *_future);
  static std::string getColumnValue(CassIterator *_iterator, int _col_idx,
                                    std::string &_err_str);
  static CassError executeQuery(CassSession *session, const char *query,
                                std::string &_err_str);
  static CassError upsertWebPage(CassSession *_session, const char *_url,
                                 const char *_content);
  static void selectWebPage(CassSession *_session, const char *_url,
                            std::string &_content);
  static void fullScanWebPages(CassSession *_session, ScanPageCb _cb);
};

} // namespace contentv1