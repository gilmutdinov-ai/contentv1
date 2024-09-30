#pragma once

#include "core/Types.h"
#include "json/json.h"
#include <vector>

namespace contentv1 {

class QueryGroup {
public:
  GroupId gid;
  std::vector<Query> queries;

  QueryGroup(const GroupId &_gid, const std::vector<Query> &_queries);
  QueryGroup() = default;
  QueryGroup(const std::string &_dump);
  [[nodiscard]] std::string dump() const;
  [[nodiscard]] std::string asJson() const;
  [[nodiscard]] Json::Value toJsonValue() const;

  [[nodiscard]] static QueryGroup fromJson(const std::string &_mess);
  [[nodiscard]] static QueryGroup fromJson(const Json::Value &_js);
};

bool operator==(const QueryGroup &a, const QueryGroup &b);

class QueryGroupHits {
public:
  GroupId gid;
  std::vector<Url> hits;

  void sort();
  void sortAndUniq();

  QueryGroupHits() = default;
  QueryGroupHits(const std::string &_dump);
  [[nodiscard]] std::string dump() const;
  [[nodiscard]] Json::Value toJsonValue() const;
  [[nodiscard]] std::string asJson() const;

  [[nodiscard]] static QueryGroupHits fromJson(const std::string &_mess);
  [[nodiscard]] static QueryGroupHits fromJson(const Json::Value &_js);
};

bool operator==(const QueryGroupHits &a, const QueryGroupHits &b);

} // namespace contentv1