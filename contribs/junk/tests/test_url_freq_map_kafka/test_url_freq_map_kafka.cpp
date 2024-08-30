#include "kafkawrap/UrlVisitsReader.h"
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

  std::unique_ptr<UrlVisitsReader> reader(
      new UrlVisitsReader{"req_urls_stream", {"kafka"}});
  long long limit{200};
  long long i = 0;
  reader->read(
      [&](const UrlVisit &uv) {
        // std::cout << "url: " << uv.first << " ts: "
        //<< serializeTimePoint(uv.second, "UTC: %Y-%m-%d %H:%M:%S") <<
        // std::endl;
        std::cout << "test_url_freq_map.cpp: insert" << std::endl;
        freq_map.insert(uv, now);
        std::cout << "test_url_freq_map.cpp: done" << std::endl;
        ++i;
      },
      limit);
  std::cout << "read url visits: " << i << std::endl;
  std::vector<UrlFreq> top_urls;
  freq_map.getTopUrls(top_urls);
  for (size_t i = 0; i < std::min(static_cast<size_t>(10), top_urls.size());
       ++i) {
    std::cout << top_urls[i].second << " " << top_urls[i].first << std::endl;
  }

  return 0;
}
