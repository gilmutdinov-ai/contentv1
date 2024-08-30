#include "api/v1/APIv1Wrap.h"
#include "misc/Log.h"

namespace contentv1 {

APIv1Wrap::~APIv1Wrap() {
  LOG("~APIv1Wrap start");
  if (m_query_loop)
    m_query_loop->stop();
  if (m_query_thread.joinable())
    m_query_thread.join();
  LOG("~APIv1Wrap finish");
}

APIv1Wrap::APIv1Wrap(IPageDb::Ptr _page_db, const APIv1WrapConfig &_config)
    : m_query_db(new QueryDb{_config}),
      m_query_loop(new QueryLoop{m_query_db, _page_db, _config}),
      m_api_v1(new APIv1{m_query_db, m_query_loop, _config}),
      m_query_thread(std::bind(&QueryLoop::loop, m_query_loop.get())) {}

} // namespace contentv1
