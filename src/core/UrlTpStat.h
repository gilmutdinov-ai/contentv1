#pragma once
#include "core/UrlVisit.h"

namespace contentv1 {

struct UrlTpStat {
  Url url;
  Tp tp;
  Cnt cnt;

  UrlTpStat() = default;
  UrlTpStat(const UrlVisit &_uv);
  UrlTpStat(const Url &_url, Tp _tp, Cnt _cnt);
};

} // namespace contentv1