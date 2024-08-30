#include "core/UrlTpStat.h"

namespace contentv1 {

UrlTpStat::UrlTpStat(const UrlVisit &_uv) : url{_uv.url}, tp{_uv.tp}, cnt{1} {}
UrlTpStat::UrlTpStat(const Url &_url, Tp _tp, Cnt _cnt)
    : url{_url}, tp{_tp}, cnt{_cnt} {}

} // namespace contentv1
