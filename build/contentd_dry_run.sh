#!/bin/bash
source "$(dirname "$0")/common.sh"

rm -rf /tmp/contentd_ut &&  bazel-bin/src/bin/contentd/contentd --dry_run
