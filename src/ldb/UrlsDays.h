#pragma once

#include "core/Types.h"
#include "core/UrlTpStat.h"
#include "misc/Time.h"
#include "src/proto/UrlDays.pb.h"
#include "src/proto/UrlsDays.pb.h"
#include <chrono>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

namespace contentv1 {

// Store last DAYS visits, drop older

template <int DAYS> class UrlsDays {
public:
  struct UrlDaysImpl {
    std::string url;
    std::map<std::string, Cnt> day_visits;

    void toUTSV(std::vector<UrlTpStat> &_utsv) const {

      _utsv.reserve(day_visits.size());
      auto end = day_visits.end();
      for (auto it = day_visits.begin(); it != end; ++it) {
        _utsv.push_back(
            UrlTpStat{url, misc::tp_from_day_str(it->first), it->second});
      }
    }

    UrlDaysImpl() = default;
    // UrlDaysImpl(const UrlDaysImpl &a) = default;
    // UrlDaysImpl(UrlsDays &&a) = default;
    // UrlDaysImpl &operator=(const UrlDaysImpl &) = default;
    // UrlDaysImpl &operator=(UrlDaysImpl &&) = default;
    //~UrlDaysImpl() = default;

    UrlDaysImpl(const Url &_url, bool _only_url) : url(_url) {}

    UrlDaysImpl(const std::string &_dump) {
      UrlDaysPb pb;
      if (!pb.ParseFromString(_dump))
        throw std::invalid_argument("UrlDaysImpl::UrlDaysImpl parsing");
      url = pb.url();
      for (size_t i = 0; i < pb.url_days_size(); ++i) {
        auto url_days_pb = pb.url_days(i);
        day_visits[url_days_pb.day_str()] = url_days_pb.visits();
      }
    }

    std::string dump() const {
      UrlDaysPb pb;
      pb.set_url(url);
      for (auto it = day_visits.begin(); it != day_visits.end(); ++it) {
        auto url_days_pb = pb.add_url_days();
        url_days_pb->set_day_str(it->first);
        url_days_pb->set_visits(it->second);
      }
      std::string ret;
      pb.SerializeToString(&ret);
      return ret;
    }

    void merge(const UrlDaysImpl &uds, Tp _now) {

      if (day_visits.empty()) {
        day_visits = uds.day_visits;
        return;
      }
      auto end = uds.day_visits.end();
      for (auto it = uds.day_visits.begin(); it != end; ++it) {
        day_visits[it->first] += it->second;
      }
      checkClean(_now);
    }

    void checkClean(Tp _now) {

      auto last_store_day_str = misc::get_day_str(_now - m_duration);
      //      std::cout << "checkClean last_store_day_str: " <<
      //      last_store_day_str
      //<< std::endl;

      for (auto it = day_visits.cbegin(); it != day_visits.cend();) {
        if (it->first < last_store_day_str) {
          day_visits.erase(it++);
          continue;
        }
        ++it;
      }
    }

    void print() const {
      std::cout << "UrlDaysImpl::print " << std::endl;
      std::cout << "url: " << url << std::endl;
      for (auto it = day_visits.begin(); it != day_visits.end(); ++it) {
        std::cout << " " << it->first << " " << it->second << std::endl;
      }
    }
  };

  void add(const UrlVisit &uv) { add(uv, std::chrono::system_clock::now()); }

  void add(const UrlVisit &_uv, Tp _now) {
    UrlTpStat utp{_uv};
    add(utp, _now);
  }

  void add(const UrlTpStat &_utp) { add(_utp, misc::get_now()); }

  void add(const UrlTpStat &utp, Tp _now) {
    if (_now - utp.tp > m_duration)
      return;

    auto day_key = misc::get_day_str(utp.tp);
    auto url_it = m_urls_days.find(utp.url);
    if (url_it == m_urls_days.end()) {
      m_urls_days[utp.url] = UrlDaysImpl{utp.url, true};
      url_it = m_urls_days.find(utp.url);
    }
    auto &url_days_stats = url_it->second;
    url_days_stats.day_visits[day_key] += utp.cnt;
    url_days_stats.checkClean(_now);
  }

  void merge(const UrlsDays<DAYS> &uds, Tp _now) {
    auto end = uds.m_urls_days.end();
    for (auto it = uds.m_urls_days.begin(); it != end; ++it) {
      m_urls_days[it->first].merge(it->second, _now);
    }
  }

  void print() const {
    for (auto it = m_urls_days.begin(); it != m_urls_days.end(); ++it) {
      std::cout << "UrlsDays::print url:" << it->first << std::endl;
      it->second.print();
    }
    std::cout << std::endl;
  }

  UrlsDays() = default;

  // for test only! access keys m_urls_days
  UrlsDays(const std::string &_dump) {
    UrlsDaysPb pb;
    if (!pb.ParseFromString(_dump))
      throw std::invalid_argument("UrlsDays::UrlsDays parsing");
    for (size_t i = 0; i < pb.urls_days_size(); ++i) {

      UrlDaysImpl url_days_impl{pb.urls_days(i)};
      m_urls_days[url_days_impl.url] = url_days_impl;
    }
  }

  // for test only! access keys m_urls_days
  std::string dump() const {
    UrlsDaysPb pb;
    for (auto it = m_urls_days.begin(); it != m_urls_days.end(); ++it) {
      auto urls_days_str = pb.add_urls_days();
      *urls_days_str = it->second.dump();
    }
    std::string ret;
    pb.SerializeToString(&ret);
    return ret;
  }

  Cnt size() const { return m_urls_days.size(); }

  bool empty() const { return m_urls_days.size() == 0; }

  bool getUTSV(const Url &_url, std::vector<UrlTpStat> &_utsv) const {
    auto it = m_urls_days.find(_url);
    if (it == m_urls_days.end())
      return false;
    it->second.toUTSV(_utsv);
    return true;
  }

  void iterateAll(
      std::function<void(const Url &, const std::vector<UrlTpStat> &)> _cb) {
    auto end = m_urls_days.end();
    for (auto it = m_urls_days.begin(); it != end; ++it) {
      std::vector<UrlTpStat> utsv;
      it->second.toUTSV(utsv);
      _cb(it->first, utsv);
    }
  }

  UrlsDays(const UrlsDays &a) = default; // : m_urls_days(a.m_urls_days) {}
  UrlsDays(UrlsDays &&a) = default; //: m_urls_days(std::move(a.m_urls_days)) {}
  UrlsDays &operator=(const UrlsDays &) = default;
  UrlsDays &operator=(UrlsDays &&) = default;
  ~UrlsDays() = default;

private:
  static constexpr auto m_duration = std::chrono::days{DAYS - 1};
  std::unordered_map<Url, UrlDaysImpl> m_urls_days;

  friend class UrlsDaysDb;
};

} // namespace contentv1
