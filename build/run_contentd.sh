#!/bin/bash
source "$(dirname "$0")/common.sh"

rm -rf /tmp/contentd_ut && $BAZEL_SYMLINK_PREFIX/bin/src/bin/contentd/contentd
