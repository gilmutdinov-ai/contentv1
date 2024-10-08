#include "ada.cpp"
#include "ada.h"

int main() {

  auto url = ada::parse<ada::url>("https://www.ya.ru");
  if (!url) {
    std::cout << "failure" << std::endl;
    return EXIT_FAILURE;
  }
  url->set_protocol("http");
  std::cout << url->get_protocol() << std::endl;
  std::cout << url->get_host() << std::endl;
  return EXIT_SUCCESS;
}
