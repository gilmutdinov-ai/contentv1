#pragma once
#include "core/FetchResult.h"
#include "core/GFQRecord.h"
#include "core/Types.h"
#include <memory>
#include <vector>

namespace contentv1 {

class SchedulerApi {
public:
  using Ptr = std::shared_ptr<SchedulerApi>;
  virtual ~SchedulerApi() = default;

  // tryFetchUrls
  //   можно качать прямо сейчас, никого не заддосим,
  //   передаем в curl evloop
  virtual void tryFetchUrls(const std::vector<GFQRecord> &_try_urls,
                            std::vector<GFQRecord> &_allowed_urls) = 0;

  // onFetched
  //   получили какой-то массив результатов, разбираем внутри
  virtual void onFetched(const std::vector<FetchResult> &_results) = 0;
};

}; // namespace contentv1
