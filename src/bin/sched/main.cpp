#include "bin/sched/cli/Sched.h"

int main(int argc, char **argv) {
  return contentv1::Sched{}(argc, const_cast<const char **>(argv));
}
