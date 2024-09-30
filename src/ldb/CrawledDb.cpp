#include "ldb/CrawledDb.h"
#include <sstream>

namespace contentv1 {

CrawledDbConfig::CrawledDbConfig() {
  addStrings(s_cfg_strs);
  addInts(s_cfg_ints);
}

CrawledDb::CrawledDb(const CrawledDbConfig &_config) {

  // it works :-)
  UrlsDaysDbConfig ud_cfg;
  ud_cfg[UrlsDaysDbConfig::STR_URLS_DAYS_DB_PATH] =
      _config[CrawledDbConfig::STR_CRAWLED_DB_PATH].as<std::string>();
  ud_cfg[UrlsDaysDbConfig::INT_SNAPSHOT_INTERVAL_SECS] =
      _config[CrawledDbConfig::INT_CRAWLED_DB_SNAPSHOT_INTERVAL_SECS].as<int>();
  ud_cfg[UrlsDaysDbConfig::INT_MERGE_INTERVAL_SECS] =
      _config[CrawledDbConfig::INT_CRAWLED_DB_MERGE_INTERVAL_SECS].as<int>();

  ud_cfg.onParsed();
  ud_cfg.validate("CrawledDb::CrawledDb UrlsDaysDbConfig");

  _fillEnumFinishedValues();

  m_db.reset(new UrlsDaysDb{ud_cfg});
}

void CrawledDb::_fillEnumFinishedValues() {

  const auto descr = CrawlAttemptsPb_AttemptStatus_descriptor();
  // skip
  //   ATTEMPT_STATUS_ENQUEUED = 0, -  не декрементится когда PENDING
  //   ATTEMPT_STATUS_PENDING = 1,
  for (int i = 2; i < descr->value_count(); ++i) {
    m_enum_finished_values.push_back(i);
  }
}

Cnt CrawledDb::getFinishedToday() {

  Cnt finished{0};
  for (size_t i = 0; i < m_enum_finished_values.size(); ++i)
    finished += _getTodayEvents(m_enum_finished_values[i]);
  return finished;
}

Cnt CrawledDb::getInProgress() {
  return _getTodayEvents(
             static_cast<int>(CrawlAttemptsPb::ATTEMPT_STATUS_ENQUEUED))
         //_getTodayEvents(
         //    static_cast<int>(CrawlAttemptsPb::ATTEMPT_STATUS_PENDING)) -
         - getFinishedToday();
}

Cnt CrawledDb::_getTodayEvents(int _event_id) {

  const Url url = genFakeUrlForResult(_event_id);
  std::vector<UrlTpStat> utsv;
  if (m_db->getUTSV(url, utsv) && !utsv.empty())
    return utsv[utsv.size() - 1].cnt;
  return 0;
}

Url CrawledDb::genFakeUrlForResult(int _value_id) {

  std::stringstream ss;
  ss << "https://ya.ru/" << _value_id;
  return ss.str();
}

void CrawledDb::saveResult(CrawlAttemptsPb::AttemptStatus _result) {

  // std::cout << "saveResult: " << static_cast<int>(_result) << std::endl;
  UrlVisit uv{genFakeUrlForResult(static_cast<int>(_result)), misc::get_now()};
  m_db->saveVisit(uv);
}

void CrawledDb::saveResult(CrawlAttemptsPb::AttemptStatus _result, Cnt _cnt) {

  UrlTpStat utp{genFakeUrlForResult(static_cast<int>(_result)), misc::get_now(),
                _cnt};
  m_db->saveVisitsPack(utp);
}

void CrawledDb::onFetched(const std::vector<FetchResult> &_results) {

  std::map<CrawlAttemptsPb::AttemptStatus, Cnt> status_cnt;
  for (size_t i = 0; i < _results.size(); ++i)
    ++status_cnt[_results[i].status];
  auto end = status_cnt.end();
  for (auto it = status_cnt.begin(); it != end; ++it) {
    saveResult(it->first, it->second);
  }
}

} // namespace contentv1
