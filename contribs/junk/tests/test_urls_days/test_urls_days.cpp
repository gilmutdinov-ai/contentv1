#include "db/UrlsDays.h"
#include <iostream>

using namespace contentv1;

int main() {

  UrlsDays<14> ud;
  UrlsDays<14> ud2;
  Url url = {"https://ya.ru"};
  auto now = get_now();
  for (size_t i = 30; i > 0; --i) {
    UrlVisit v(url, now - std::chrono::days{i});
    ud.add(v, now);
  }
  for (size_t i = 7; i > 0; --i) {
    UrlVisit v(url, now - std::chrono::days{i});
    ud2.add(v, now);
  }

  ud2.merge(ud, now);

  UrlsDays<14> ud3(ud2.dump());
  // ud3.print();

  assert(ud3.size() == 1);
  std::vector<UrlTpStat> utsv;
  assert(ud3.getUTSV(url, utsv));
  // check size
  assert(utsv.size() == 13);
  // check order
  assert(utsv[0].cnt == 1);
  assert(utsv[12].cnt == 2);

  // std::cout << "utsv.size: " << utsv.size() << std::endl;
  return 0;
}
