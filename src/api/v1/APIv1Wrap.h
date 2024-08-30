#pragma once
#include "api/v1/APIv1.h"

namespace contentv1 {

/*
Own & init: API, QueryLoop, QueryDb
    thread for QueryLoop
*/

class APIv1WrapConfig : public virtual APIv1Config,
                        public virtual QueryLoopConfig,
                        public virtual QueryDbConfig {
public:
};

class APIv1Wrap {
public:
  using Ptr = std::shared_ptr<APIv1Wrap>;
  APIv1Wrap(IPageDb::Ptr _page_db, const APIv1WrapConfig &_config);
  ~APIv1Wrap();

private:
  QueryDb::Ptr m_query_db;
  QueryLoop::Ptr m_query_loop;
  APIv1::Ptr m_api_v1;
  std::thread m_query_thread;
};

} // namespace contentv1
