#include "PageDbTester.h"

namespace contentv1 {
PageDbTester::PageDbTester() {

  const std::string db_path{"/tmp/db_ut/pagedb"};
  std::filesystem::remove_all(db_path);

  PageDbConfig cfg;
  cfg[PageDbConfig::STR_PAGE_DB_PATH] = db_path;
  cfg.onParsed();
  cfg.validate("PageDbTester::PageDbTester");

  PageDb db{cfg};

  auto page1 = std::make_pair(Url{"https://ya.ru"}, "_ya_content_");
  auto page2 = std::make_pair(Url{"https://mail.ru"}, "_mail_content_");

  db.save(page1.first, page1.second);
  db.save(page2.first, page2.second);

  std::string page1_loaded, page2_loaded;
  db.load(page1.first, page1_loaded);
  db.load(page2.first, page2_loaded);

  REQUIRE(page1.second == page1_loaded);
  REQUIRE(page2.second == page2_loaded);

  {
    std::map<Url, std::string> expect_pages;
    expect_pages.insert(page1);
    expect_pages.insert(page2);

    std::map<Url, std::string> got_pages;
    db.scanFull([&](const Url &_url, const std::string &_content) {
      got_pages[_url] = _content;
    });
    REQUIRE(expect_pages == got_pages);
  }
}
} // namespace contentv1
