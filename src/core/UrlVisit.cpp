#include "core/UrlVisit.h"
#include "misc/Time.h"
#include "simdjson.h"
#include "json/json.h"

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

std::string UrlVisit::asJs() const {

  Json::Value js;
  js["url"] = url;
  js["ts"] = misc::to_int64(tp);
  return Json::FastWriter{}.write(js);
}

} // namespace contentv1
