#!/bin/bash
source "$(dirname "$0")/common.sh"

rm -rf /tmp/sched_ut && $BAZEL_SYMLINK_PREFIX/bin/src/bin/sched/sched --dry_run
