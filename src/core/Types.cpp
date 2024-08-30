#include "core/Types.h"
#include "misc/Time.h"
#include <unordered_map>

namespace contentv1 {
void reduceUrlFreqs(const std::vector<UrlFreq> &_in,
                    std::vector<UrlFreq> &_out) {
  std::unordered_map<Url, UrlFreq> u_map;
  for (size_t i = 0; i < _in.size(); ++i) {
    auto it = u_map.find(_in[i].first);
    if (it == u_map.end())
      u_map[_in[i].first] = _in[i];
    else
      u_map[_in[i].first].second += _in[i].second;
  }
  for (auto it : u_map)
    _out.push_back(it.second);
}

} // namespace contentv1