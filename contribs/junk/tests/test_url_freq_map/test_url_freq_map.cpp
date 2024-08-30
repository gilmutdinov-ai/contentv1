#include "kafkawrap/UrlVisitsReader.h"
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "core/UrlFreqMap.h"

using namespace contentv1;

using time_point = std::chrono::system_clock::time_point;
std::string serializeTimePoint(const time_point &time,
                               const std::string &format) {
  std::time_t tt = std::chrono::system_clock::to_time_t(time);
  std::tm tm = *std::gmtime(&tt); // GMT (UTC)
  // std::tm tm = *std::localtime(&tt); //Locale time-zone, usually UTC by
  // default.
  std::stringstream ss;
  ss << std::put_time(&tm, format.c_str());
  return ss.str();
}

int main() {

  UrlFreqMap freq_map{std::chrono::days{7}};

  auto now = std::chrono::system_clock::now();
  Url url{"https://ya.ru"};

  for (size_t i = 30; i > 0; --i) {
    UrlVisit v(url, now - std::chrono::days{i});
    freq_map.insert(v, now);
    if (i < 7)
      freq_map.insert(v, now);
  }

  Url url2{"https://mail.ru"};
  {
    UrlVisit v(url2, now - std::chrono::days{1});
    freq_map.insert(v, now);
  }

  std::vector<UrlFreq> top_urls;
  freq_map.getTopUrls(top_urls);
  for (size_t i = 0; i < std::min(static_cast<size_t>(10), top_urls.size());
       ++i) {
    std::cout << top_urls[i].second << " " << top_urls[i].first << std::endl;
  }
  assert(!top_urls.empty());
  assert(top_urls[0].first == url);
  assert(top_urls[1].first == url2);

  return 0;
}
