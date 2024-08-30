#include "ldb/HostDb.h"
#include <filesystem>
#include <unordered_set>

namespace contentv1 {

HostDbConfig::HostDbConfig() { addStrings(s_cfg_strings); }

HostDb::HostDb(const HostDbConfig &_config) : m_limits(_config) {

  auto _path = _config[HostDbConfig::STR_HOST_DB_PATH].asString();

  std::filesystem::create_directories(_path);

  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, _path.c_str(), &m_db);
  if (!status.ok())
    throw std::runtime_error("HostDb::HostDb leveldb !ok " + status.ToString());
  _loadRobotsDb();
}

void HostDb::_loadRobotsDb() {
  std::unique_ptr<leveldb::Iterator> it{
      m_db->NewIterator(leveldb::ReadOptions())};
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    m_host_robots[it->key().ToString()] = Robots{it->value().ToString()};
  }
  if (!it->status().ok()) {
    std::cerr << "HostDb::_loadRobotsDb scan error\n";
  }
}

HostDb::~HostDb() { delete m_db; }
/*
void HostDb::onCrawled(const std::vector<Url> &_urls) {
  std::lock_guard<std::mutex> lock(m_mtx);
  for (size_t i = 0; i < _urls.size(); ++i) {
    UrlParsed url_parsed{_urls[i]};
    if (!url_parsed.ok())
      continue;
    m_host_freq[url_parsed.host()].decCrawling();
    m_global_freq.decCrawling();
  }
}*/

void HostDb::onFetched(const std::vector<FetchResult> &_results) {

  std::lock_guard<std::mutex> lock(m_mtx);
  for (size_t i = 0; i < _results.size(); ++i) {
    UrlParsed url_parsed{_results[i].gfq.url};
    if (!url_parsed.ok())
      continue;
    m_host_freq[url_parsed.host()].decCrawling();
    m_global_freq.decCrawling();

    if (_results[i].isRobots() && _results[i].isOk()) {
      auto robots_str = _results[i].content;
      m_host_robots[url_parsed.host()] = Robots{robots_str};

      auto s =
          m_db->Put(leveldb::WriteOptions(), url_parsed.host(), robots_str);
      if (!s.ok()) {
        std::cerr << "HostDb::onRobotsTxt could not write leveldb\n";
      }
    }
  }
}

// for tests only
void HostDb::onRobotsTxt(const Url &_robots_url,
                         const std::string &_robots_str) {

  std::lock_guard<std::mutex> lock(m_mtx);
  UrlParsed url_parsed{_robots_url};
  if (!url_parsed.ok())
    return;

  m_global_freq.decCrawling();
  m_host_freq[url_parsed.host()].decCrawling();
  m_host_robots[url_parsed.host()] = Robots{_robots_str};

  auto s = m_db->Put(leveldb::WriteOptions(), url_parsed.host(), _robots_str);
  if (!s.ok()) {
    std::cerr << "HostDb::onRobotsTxt could not write leveldb\n";
  }
}

void HostDb::leaveAllowedByRobots(const std::vector<UrlFreq> &_urls,
                                  std::vector<UrlFreq> &_allowed_urls,
                                  std::vector<UrlFreq> &_need_robots) {
  std::lock_guard<std::mutex> lock(m_mtx);
  for (size_t i = 0; i < _urls.size(); ++i) {
    UrlParsed url_parsed{_urls[i].first};
    if (!url_parsed.ok())
      continue;
    std::string need_robots;
    if (_isAllowedRobots(url_parsed, need_robots)) {
      _allowed_urls.push_back(
          UrlFreq{url_parsed.normalized(), _urls[i].second});
    } else if (!need_robots.empty()) {
      _need_robots.push_back(UrlFreq{need_robots, _urls[i].second});
    }
  }
}

std::vector<HostDb::EHostCrawlDecision>
HostDb::tryCrawl(const std::vector<Url> &_try_urls, Tp now,
                 std::vector<Url> &_allowed_urls,
                 std::vector<Url> &_need_robots) {
  std::lock_guard<std::mutex> lock(m_mtx);

  std::vector<EHostCrawlDecision> crawl_decisions;
  std::unordered_set<Url> need_robots_set;

  for (size_t i = 0; i < _try_urls.size(); ++i) {

    UrlParsed url_parsed{_try_urls[i]};
    if (!url_parsed.ok()) {
      crawl_decisions.push_back(DISALLOW_URL_PARSING);
      continue;
    }

    Url need_robots;
    EHostCrawlDecision decision = _tryCrawl(url_parsed, need_robots);
    crawl_decisions.push_back(decision);

    if (decision == ALLOW) {
      _allowed_urls.push_back(url_parsed.normalized());
    } else if (decision == DISALLOW_NEED_ROBOTS) {
      need_robots_set.insert(need_robots);
    }
  }
  _need_robots =
      std::vector<Url>{need_robots_set.begin(), need_robots_set.end()};
  return crawl_decisions;
}

std::vector<HostDb::EHostCrawlDecision>
HostDb::tryCrawl(const std::vector<GFQRecord> &_try_urls, Tp now,
                 std::vector<GFQRecord> &_allowed_urls,
                 std::vector<Url> &_need_robots) {

  std::lock_guard<std::mutex> lock(m_mtx);

  std::vector<EHostCrawlDecision> crawl_decisions;
  std::unordered_set<Url> need_robots_set;

  for (size_t i = 0; i < _try_urls.size(); ++i) {

    UrlParsed url_parsed{_try_urls[i].url};
    if (!url_parsed.ok()) {
      crawl_decisions.push_back(DISALLOW_URL_PARSING);
      continue;
    }

    Url need_robots;
    EHostCrawlDecision decision = _tryCrawl(url_parsed, need_robots);
    crawl_decisions.push_back(decision);

    if (decision == ALLOW) {
      _allowed_urls.push_back(_try_urls[i]);
    } else if (decision == DISALLOW_NEED_ROBOTS) {
      need_robots_set.insert(need_robots);
    }
  }
  _need_robots =
      std::vector<Url>{need_robots_set.begin(), need_robots_set.end()};
  return crawl_decisions;
}

bool HostDb::hasRobots(const Url &url) {
  std::lock_guard<std::mutex> lock(m_mtx);
  UrlParsed url_parsed{url};
  if (!url_parsed.ok())
    return false;
  return m_host_robots.find(url_parsed.host()) != m_host_robots.end();
}

Cnt HostDb::sizeRobots() {
  std::lock_guard<std::mutex> lock(m_mtx);
  return m_host_robots.size();
}

/////////// PRIVATE

HostDb::EHostCrawlDecision HostDb::_tryCrawl(const UrlParsed &_url_parsed,
                                             Url &need_robots) {

  if (!_isAllowedRobots(_url_parsed, need_robots)) {
    if (!need_robots.empty())
      return DISALLOW_NEED_ROBOTS;
    return DISALLOW_BY_ROBOTS;
  }

  auto host = _url_parsed.host();
  if (!_isAllowedFreq(host))
    return DISALLOW_FREQ;

  _setCrawling(host);
  return ALLOW;
}

bool HostDb::_isAllowedRobots(const UrlParsed &_url_parsed,
                              Url &need_robots) const {
  // если урл и есть роботс, то разрешено с тз роботс
  if (_url_parsed.isRobots())
    return true;

  auto it = m_host_robots.find(_url_parsed.host());
  if (it == m_host_robots.end()) {
    need_robots = _url_parsed.robots();
    return false;
  }
  return it->second.isAllowed(_url_parsed.normalized());
}

bool HostDb::_isAllowedFreq(const Host &_host) {

  auto it = m_host_freq.find(_host);
  if (it == m_host_freq.end())
    return true;
  return it->second.isAllowed(m_limits) && m_global_freq.isAllowed(m_limits);
}

void HostDb::_setCrawling(const Host &_host) {

  m_host_freq[_host].incCrawling();
  m_global_freq.incCrawling();
}
} // namespace contentv1
