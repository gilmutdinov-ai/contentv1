#include "CrawledDbTester.h"

namespace contentv1 {
CrawledDbTester::CrawledDbTester() {

  const std::string crawled_db_path{"/tmp/test_crawled_db"};

  // events from previous tests saved :-)
  std::filesystem::remove_all(crawled_db_path);
  CrawledDbConfig cfg;

  cfg[CrawledDbConfig::STR_CRAWLED_DB_PATH] = crawled_db_path;
  cfg[CrawledDbConfig::INT_CRAWLED_DB_MERGE_INTERVAL_SECS] = 2;
  cfg[CrawledDbConfig::INT_CRAWLED_DB_SNAPSHOT_INTERVAL_SECS] = 3;

  cfg.onParsed();
  cfg.validate();

  CrawledDb db{cfg};
  Cnt not_finished_events = 32;
  Cnt expect_finished_count = 128;

  for (size_t j = 0; j < not_finished_events; ++j) {
    db.saveResult(CrawlAttemptsPb::ATTEMPT_STATUS_ENQUEUED);
    db.saveResult(CrawlAttemptsPb::ATTEMPT_STATUS_PENDING);
  }

  for (size_t j = 0; j < expect_finished_count; ++j) {
    if (j % 2 == 0)
      db.saveResult(CrawlAttemptsPb::ATTEMPT_STATUS_OK);
    else
      db.saveResult(CrawlAttemptsPb::ATTEMPT_STATUS_5xx);
  }

  sleep(3);

  auto got_finished_count = db.getFinishedToday();
  REQUIRE(got_finished_count == expect_finished_count);
}
} // namespace contentv1