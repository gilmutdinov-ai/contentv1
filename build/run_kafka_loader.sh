#!/bin/bash
source "$(dirname "$0")/common.sh"

$BAZEL_SYMLINK_PREFIX/bin/src/bin/kafka_loader/kafka_loader
