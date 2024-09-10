#include "scylla/PageDbScylla.h"
#include "misc/Log.h"

namespace contentv1 {

PageDbScyllaConfig::PageDbScyllaConfig() { addArrays(s_cfg_arrays); }

PageDbScylla::PageDbScylla(const PageDbScyllaConfig &_config) {
#warning NOT_IMPLEMENTED
  m_cluster = cass_cluster_new();
  m_session = cass_session_new();

  std::string hosts = misc::Config::renderArray(
      _config[PageDbScyllaConfig::STR_SCYLLA_HOSTS], ',');

  cass_cluster_set_contact_points(m_cluster, hosts.c_str());
  m_connect_future = cass_session_connect(m_session, m_cluster);

  if (cass_future_error_code(m_connect_future) != CASS_OK) {

    cass_future_free(m_connect_future);
    cass_session_free(m_session);
    cass_cluster_free(m_cluster);

    throw std::invalid_argument("PageDbScylla::PageDbScylla connect " + hosts);
  }
}

PageDbScylla::~PageDbScylla() {
  if (m_connect_future)
    cass_future_free(m_connect_future);
  if (m_session)
    cass_session_free(m_session);
  if (m_cluster)
    cass_cluster_free(m_cluster);
}

void PageDbScylla::save(const Url &_url, const std::string &_content) {
  PageDbScylla::upsertWebPage(m_session, _url.c_str(), _content.c_str());
}

bool PageDbScylla::load(const Url &_url, std::string &_content) {
  try {
    selectWebPage(m_session, _url.c_str(), _content);
  } catch (std::exception &_ex) {
    LOG(_ex.what());
    return false;
  }
  return true;
}

void PageDbScylla::scanFull(ScanPageCb _cb) {

  try {
    fullScanWebPages(m_session, _cb);
  } catch (std::exception &_ex) {
    LOG("scanFull" << _ex.what());
  }
}

} // namespace contentv1
