#pragma once

#include <chrono>
#include <stdint.h>
#include <string>
#include <vector>

namespace contentv1 {

// using Duration = std::chrono::system_clock::duration;
using DurationSec = std::chrono::seconds;
using Tp = std::chrono::time_point<std::chrono::system_clock>;
using Ts = int64_t;
using Url = std::string;
using Host = std::string;

using Cnt = int64_t;
using UrlFreq = std::pair<Url, Cnt>;

using GroupId = std::string;
using Query = std::string;

void reduceUrlFreqs(const std::vector<UrlFreq> &_in,
                    std::vector<UrlFreq> &_out);

} // namespace contentv1
