#include "bin/contentd/cli/Contentd.h"

int main(int argc, char **argv) {
  return contentv1::Contentd{}(argc, const_cast<const char **>(argv));
}
