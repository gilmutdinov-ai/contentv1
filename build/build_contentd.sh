#!/bin/bash
source "$(dirname "$0")/common.sh"

echo "bazel symlink prefix: ${BAZEL_SYMLINK_PREFIX}"

CC=clang bazel build $BAZEL_CACHE_OPT $BAZEL_REPO_CACHE_OPT $BAZEL_REGISTRY_OPT --symlink_prefix $BAZEL_SYMLINK_PREFIX --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --strip=never  //src/bin/contentd
