#pragma once

#include "core/Types.h"
#include <functional>

namespace contentv1 {

void test_visits_x(size_t _x, const Url &_url, Tp _now,
                   std::function<void(const UrlVisit &uv)> _cb) {

  for (size_t i = 30; i > 0; --i) {
    UrlVisit v(_url, _now - std::chrono::days{i});

    for (size_t j = 0; j < _x; ++j)
      _cb(v);

    if (i < 7)
      for (size_t j = 0; j < _x; ++j)
        _cb(v);
  }
}

void test_visits(const Url &_url, Tp _now,
                 std::function<void(const UrlVisit &uv)> _cb) {

  test_visits_x(1, _url, _now, _cb);
}

void test_visits_unsorted_x(size_t _x, const Url &_url, Tp _now,
                            std::function<void(const UrlVisit &uv)> _cb) {

  for (size_t i = 0; i < 30; ++i) {
    UrlVisit v(_url, _now - std::chrono::days{i});

    for (size_t j = 0; j < _x; ++j)
      _cb(v);
  }

  for (size_t i = 0; i < 7; ++i) {
    UrlVisit v(_url, _now - std::chrono::days{i});
    _cb(v);
  }
}

void test_visits_unsorted(const Url &_url, Tp _now,
                          std::function<void(const UrlVisit &uv)> _cb) {

  test_visits_unsorted_x(1, _url, _now, _cb);
}

} // namespace contentv1