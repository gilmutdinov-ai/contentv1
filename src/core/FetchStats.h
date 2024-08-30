#pragma once

#include "core/Types.h"

namespace contentv1 {

struct FetchStats {
  Cnt pushed;
  Cnt success;
  Cnt fail;
  Cnt robots_success;
  Cnt robots_fail;
};

bool operator==(const FetchStats &a, const FetchStats &b);

} // namespace contentv1
