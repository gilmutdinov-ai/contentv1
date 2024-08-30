#pragma once
#include "core/Types.h"

namespace contentv1 {

class UrlVisit {
public:
  Url url;
  Tp tp;

  UrlVisit(const Url &_url, Tp _tp);
  UrlVisit(const std::string &_json);
  UrlVisit() = default;
};

} // namespace contentv1