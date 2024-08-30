#include "misc/Time.h"
#include <cassert>
#include <iostream>

using namespace contentv1;

int main() {
  auto win =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::days{7});
  auto now = get_now();
  auto tp1 = now - std::chrono::days{14};
  auto dif = std::chrono::duration_cast<std::chrono::seconds>(now - tp1);
  /*std::cout << "now:" << to_readable(now) << std::endl;
  std::cout << "tp1:" << to_readable(tp1) << std::endl;
  std::cout << "dif:" << to_readable_dur(dif) << std::endl;
  std::cout << "win:" << to_readable_dur(win) << std::endl;
  std::cout << "dif > win:" << (dif > win) << std::endl;*/

  assert(dif > win);

  auto day_str = get_day_str(now);
  auto day_str_restored = get_day_str(tp_from_day_str(day_str));
  /*
  std::cout << "day_ori: " << day_str << std::endl;
  std::cout << "day_res: " << day_str_restored << std::endl;
*/
  assert(day_str == day_str_restored);

  return 0;
}
