#!/bin/bash
source "$(dirname "$0")/common.sh"

set -e 

#CC=clang bazel build --cxxopt=-std=c++20 --copt="-g" --distdir build //contribs/junk/tests/test_kafka_reader:test_kafka_reader &&
#./bazel-bin/contribs/junk/tests/test_kafka_reader/test_kafka_reader

#CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_grpc/cpp_grpc_library:cpp_grpc_library

CC=clang bazel build $BAZEL_CACHE_OPT $BAZEL_REPO_CACHE_OPT $BAZEL_REGISTRY_OPT --experimental_enable_bzlmod --symlink_prefix $BAZEL_SYMLINK_PREFIX --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_grpc/test_async_client

$BAZEL_SYMLINK_PREFIX/bazel-bin/contribs/junk/tests/test_grpc/test_async_client/test_async_client
