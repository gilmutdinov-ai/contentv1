#include "core/UrlVisit.h"
#include "simdjson.h"

namespace contentv1 {

UrlVisit::UrlVisit(const Url &_url, Tp _tp) : url(_url), tp(_tp) {}

UrlVisit::UrlVisit(const std::string &_json) {
  simdjson::ondemand::parser parser;
  simdjson::padded_string json{_json};
  auto doc = parser.iterate(json);
  std::string_view url_v = doc["url"];
  url = url_v;
  uint64_t ts = doc["ts"];
  tp = std::chrono::system_clock::time_point{std::chrono::seconds{ts}};
}

} // namespace contentv1
