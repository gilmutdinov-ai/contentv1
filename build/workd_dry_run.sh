#!/bin/bash
source "$(dirname "$0")/common.sh"

rm -rf /tmp/workd_ut && $BAZEL_SYMLINK_PREFIX/bin/src/bin/workd/workd --dry_run
