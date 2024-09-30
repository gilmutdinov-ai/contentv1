#include "bin/workd/cli/Workd.h"

int main(int argc, char **argv) {
  return contentv1::Workd{}(argc, const_cast<const char **>(argv));
}
