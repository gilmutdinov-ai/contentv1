#include "bin/contentd/cli/ut/ContentdTester.h"
#include "bin/contentd/cli/Contentd.h"
#include <filesystem>

namespace contentv1 {
ContentdTester::ContentdTester() {
  std::filesystem::remove_all("/tmp/contentd_ut");
  int argc = 2;
  char *argv[2];
  argv[0] = "bazel-bin/src/bin/contentd/contentd";
  argv[1] = "--config_help";
  {
    Contentd d{};
    REQUIRE(d(argc, argv) == 0);
  }
}
} // namespace contentv1