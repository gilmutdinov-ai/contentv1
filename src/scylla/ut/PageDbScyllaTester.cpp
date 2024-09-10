#include "PageDbScyllaTester.h"

namespace contentv1 {
PageDbScyllaTester::PageDbScyllaTester() {

  PageDbScyllaConfig cfg;

  std::string hosts{"scylla-docker-scylla-contentv1-1"};

  cfg[PageDbScyllaConfig::STR_SCYLLA_HOSTS].append(hosts);
  cfg.onParsed();
  cfg.validate();

  PageDbScylla db{cfg};

  db.deleteAll();

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