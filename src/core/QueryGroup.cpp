#include "QueryGroup.h"
#include "src/proto/QueryGroup.pb.h"
#include <algorithm>
#include <stdexcept>

namespace contentv1 {

/// QUERIES

QueryGroup::QueryGroup(const GroupId &_gid, const std::vector<Query> &_queries)
    : gid{_gid}, queries{_queries} {}

QueryGroup::QueryGroup(const std::string &_dump) {

  QueryGroupPb pb;
  if (!pb.ParseFromString(_dump))
    throw std::invalid_argument("QueryGroup::QueryGroup parsing");

  gid = pb.gid();
  for (size_t i = 0; i < pb.queries_size(); ++i) {
    queries.push_back(pb.queries(i));
  }
}

std::string QueryGroup::dump() const {

  QueryGroupPb pb;
  pb.set_gid(gid);
  for (size_t i = 0; i < queries.size(); ++i) {
    auto pb_query = pb.add_queries();
    *pb_query = queries[i];
  }
  return pb.SerializeAsString();
}

Json::Value QueryGroup::toJsonValue() const {
  Json::Value js;
  js["gid"] = gid;
  Json::Value queries_js;
  for (size_t i = 0; i < queries.size(); ++i)
    queries_js.append(queries[i]);
  js["queries"] = queries_js;
  return js;
}

std::string QueryGroup::asJson() const {
  return Json::FastWriter{}.write(toJsonValue());
}

QueryGroup QueryGroup::fromJson(const std::string &_mess) {

  Json::Value js;
  if (!Json::Reader().parse(_mess, js))
    throw std::invalid_argument("QueryGroup::fromJson parsing error");
  return QueryGroup::fromJson(js);
}

QueryGroup QueryGroup::fromJson(const Json::Value &_js) {
  static std::string err_msg = "QueryGroup::fromJson parsing error";

  QueryGroup ret;
  if (!_js["gid"].isString())
    throw std::invalid_argument(err_msg);
  ret.gid = _js["gid"].asString();

  if (_js["queries"].isNull())
    return ret;

  if (!_js["queries"].isArray())
    throw std::invalid_argument(err_msg);

  for (Json::Value::ArrayIndex i = 0; i < _js["queries"].size(); ++i) {
    if (!_js["queries"][i].isString())
      throw std::invalid_argument(err_msg);
    ret.queries.push_back(_js["queries"][i].asString());
  }
  return ret;
}

bool operator==(const QueryGroup &a, const QueryGroup &b) {
  if (a.gid != b.gid)
    return false;
  if (a.queries.size() != b.queries.size())
    return false;
  for (size_t i = 0; i < a.queries.size(); ++i)
    if (a.queries[i] != b.queries[i])
      return false;
  return true;
}

/// HITS

void QueryGroupHits::sort() { std::sort(hits.begin(), hits.end()); }

void QueryGroupHits::sortAndUniq() {
  std::set<Url> hits_s(hits.begin(), hits.end());
  hits.clear();
  std::copy(hits_s.begin(), hits_s.end(), std::back_inserter(hits));
}

QueryGroupHits::QueryGroupHits(const std::string &_dump) {
  QueryGroupHitsPb pb;
  if (!pb.ParseFromString(_dump))
    throw std::invalid_argument("QueryGroupHitsPb::QueryGroupHitsPb parsing");

  gid = pb.gid();
  for (size_t i = 0; i < pb.hits_size(); ++i) {
    hits.push_back(pb.hits(i));
  }
}

std::string QueryGroupHits::dump() const {
  QueryGroupHitsPb pb;
  pb.set_gid(gid);
  for (size_t i = 0; i < hits.size(); ++i) {
    auto pb_hit = pb.add_hits();
    *pb_hit = hits[i];
  }
  return pb.SerializeAsString();
}

Json::Value QueryGroupHits::toJsonValue() const {
  Json::Value js;
  js["gid"] = gid;
  Json::Value hits_js;
  for (size_t i = 0; i < hits.size(); ++i)
    hits_js.append(hits[i]);
  js["hits"] = hits_js;
  return js;
}

std::string QueryGroupHits::asJson() const {
  return Json::FastWriter{}.write(toJsonValue());
}

QueryGroupHits QueryGroupHits::fromJson(const std::string &_mess) {

  Json::Value js;
  if (!Json::Reader().parse(_mess, js))
    throw std::invalid_argument("QueryGroupHits::fromJson parsing error");
  return QueryGroupHits::fromJson(js);
}

QueryGroupHits QueryGroupHits::fromJson(const Json::Value &_js) {
  static std::string err_msg = "QueryGroupHits::fromJson parsing error";

  QueryGroupHits ret;
  if (!_js["gid"].isString())
    throw std::invalid_argument(err_msg);
  ret.gid = _js["gid"].asString();

  if (_js["hits"].isNull())
    return ret;

  if (!_js["hits"].isArray())
    throw std::invalid_argument(err_msg);

  for (Json::Value::ArrayIndex i = 0; i < _js["hits"].size(); ++i) {
    if (!_js["hits"][i].isString())
      throw std::invalid_argument(err_msg);
    ret.hits.push_back(_js["hits"][i].asString());
  }
  return ret;
}

bool operator==(const QueryGroupHits &a, const QueryGroupHits &b) {
  if (a.gid != b.gid)
    return false;
  if (a.hits.size() != b.hits.size())
    return false;
  for (size_t i = 0; i < a.hits.size(); ++i)
    if (a.hits[i] != b.hits[i])
      return false;
  return true;
}

} // namespace contentv1
