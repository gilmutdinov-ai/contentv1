#!/bin/bash
source "$(dirname "$0")/common.sh"

set -e

#CC=clang bazel build --cxxopt=-std=c++20 --copt="-g" --distdir build //contribs/junk/tests/test_kafka_writer:test_kafka_writer &&
#./bazel-bin/contribs/junk/tests/test_kafka_writer/test_kafka_writer

#CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_fetch_loop:test_fetch_loop &&
#gdb ./bazel-bin/contribs/junk/tests/test_fetch_loop/test_fetch_loop

#CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_grpc/cpp_proto_compile:cpp_proto_compile
# && gdb --args ./bazel-bin/contribs/junk/tests/test_grpc/test_grpc

#CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_grpc/cpp_grpc_compile:cpp_grpc_compile

#CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_grpc/cpp_proto_library:cpp_proto_library

#CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_grpc/test_async_server

#./bazel-bin/contribs/junk/tests/test_grpc/test_async_server/test_async_server

#CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_scylla

#./bazel-bin/contribs/junk/tests/test_scylla/test_scylla 10.0.4.16

#CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_pagedb_scylla

#./bazel-bin/contribs/junk/tests/test_pagedb_scylla/test_pagedb_scylla 10.0.4.16

CC=clang bazel test --symlink_prefix $BAZEL_SYMLINK_PREFIX --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //contribs/junk/tests/test_pagedb_scylla

