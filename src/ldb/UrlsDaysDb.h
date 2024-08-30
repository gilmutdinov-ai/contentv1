#pragma once
#include "UrlsDays.h"
#include "concurrentqueue.h"
#include "core/Types.h"
#include "leveldb/db.h"
#include "misc/Config.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "core/UrlFreqStatsLF.h"

namespace contentv1 {

/*
 Чтобы в EnqeueLoop принимать решения что качать повторно, и чтобы
инициализировать при старте UrlFreqMap у EnqeueLoop, куда потом направляется
поток

За две недели хранить схлопнуто по дням
Сериализовать/десериализовать

LoadLoop непрерывно льёт saveVisit
При старте отсюда конструируются UrlFreqMaps

Внутри lock-free queue
  https://github.com/cameron314/concurrentqueue

фоновый поток сливает визиты в leveldb и
 обновляет онлайн структуру через rwlock
*/

class UrlsDaysDbConfig : virtual public misc::Config {
public:
  inline static const std::string STR_URLS_DAYS_DB_PATH{
      "sched_urls_days_db_path"};
  inline static const std::string INT_SNAPSHOT_INTERVAL_SECS{
      "sched_urls_days_db_snapshot_interval_secs"};
  inline static const std::string INT_MERGE_INTERVAL_SECS{
      "sched_urls_days_db_merge_interval_secs"};

  // inline static const std::string INT_URL_PERS_STATS_STORE_DAYS{
  //"url_days_store_days"
private:
  const std::vector<std::string> s_cfg_strs{STR_URLS_DAYS_DB_PATH};
  const std::vector<std::string> s_cfg_ints{INT_SNAPSHOT_INTERVAL_SECS,
                                            INT_MERGE_INTERVAL_SECS};

public:
  UrlsDaysDbConfig() {
    addStrings(s_cfg_strs);
    addInts(s_cfg_ints);
  }
};

class UrlsDaysDb {
public:
  using Ptr = std::shared_ptr<UrlsDaysDb>;

private:
  using UrlsLastDays = UrlsDays<14>;

  const std::string m_db_path;
  const int m_snapshot_interval_secs;
  const int m_merge_interval_secs;

  std::atomic<bool> m_running;

  moodycamel::ConcurrentQueue<UrlTpStat> m_q;
  std::unique_ptr<std::thread> m_merge_th;

  // global urls days
  std::shared_mutex m_mtx;
  std::shared_ptr<UrlsLastDays> m_urls_days;

  std::unique_ptr<std::thread> m_snap_th;
  leveldb::DB *m_db;

  // for waiting current merge finished
  std::mutex m_merge_mtx;
  std::condition_variable m_merge_cv;
  bool m_merge_finished{false};

private:
  void _mergeThread();
  void _mergeEventsFromQueue();
  void _setMergeStarted();
  void _setMergeFinished();

  void _snapThread();
  void _openLevelDb();
  void _loadSnapshot();
  void _saveSnapshot();

public:
  UrlsDaysDb(const UrlsDaysDbConfig &_config);
  ~UrlsDaysDb();

  // WRITE
  void saveVisit(const UrlVisit &_url_visit);
  void saveVisitsPack(const UrlTpStat &_utp);

  // READ
  Cnt size();
  void print();
  bool getUTSV(const Url &_url, std::vector<UrlTpStat> &_utsv);

  void iterateAll(
      std::function<void(const Url &, const std::vector<UrlTpStat> &)> _cb);

  // wait to merge real data
  void waitMerged();

  friend bool operator==(const UrlsDaysDb &a, const UrlsDaysDb &b);
};

bool operator==(const UrlsDaysDb &a, const UrlsDaysDb &b);

} // namespace contentv1