#include "core/FetchStats.h"

namespace contentv1 {

bool operator==(const FetchStats &a, const FetchStats &b) {
  if (a.success != b.success)
    return false;
  if (a.fail != b.fail)
    return false;
  if (a.robots_success != b.robots_success)
    return false;
  if (a.robots_fail != b.robots_fail)
    return false;
  return true;
}

} // namespace contentv1
