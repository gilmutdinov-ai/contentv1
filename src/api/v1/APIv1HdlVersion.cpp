#include "api/v1/APIv1.h"

namespace contentv1 {

void APIv1::hdlVersion(const APIRequest &_api_req, APIResponse &_api_res) {
  _api_res.set(s_version_json_str);
}

} // namespace contentv1
