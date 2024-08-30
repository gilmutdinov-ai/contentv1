#include "core/UrlParsed.h"
#include <iostream>

int main() {
  auto url = contentv1::UrlParsed("https://www.ya.ru");
  if (!url.ok()) {
    std::cout << "url parsing error\n";
    return 1;
  }
  std::cout << "normalized: " << url.normalized() << std::endl;
  // std::cout << "resersed: " << url.reversed() << std::endl;
  std::cout << "host: " << url.host() << std::endl;

  return 0;
}
