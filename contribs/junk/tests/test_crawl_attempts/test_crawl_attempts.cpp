#include "core/CrawlAttempts.h"
#include <cassert>
#include <iostream>

using namespace contentv1;

int main() {
  CrawlAttempts a;

  Tp tp1 = a.add();
  Tp tp2 = a.add();
  a.set(tp1, CrawlAttemptsPb::ATTEMPT_RESULT_OK);

  {
    CrawlAttempts b(a.dump());
    std::cout << "a: " << a.size() << std::endl;
    std::cout << "b: " << b.size() << std::endl;
    std::cout << "is a==b: " << (a == b) << std::endl;
  }
  return 0;
}
