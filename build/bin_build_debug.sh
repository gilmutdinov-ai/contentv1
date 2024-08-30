#!/bin/bash
source "$(dirname "$0")/common.sh"

rm bazel-bin
rm bazel-out

CC=clang bazel build --cxxopt=-std=c++20 --copt="-g" --distdir build //contribs/junk/tests/test_hello:hello &&
CC=clang bazel build --cxxopt=-std=c++20 --copt="-g" --distdir build //contribs/junk/tests/test_kafka:test_kafka &&
CC=clang bazel build --cxxopt=-std=c++20 --copt="-g" --distdir build //contribs/junk/tests/test_kafka_reader:test_kafka_reader &&
CC=clang bazel build --cxxopt=-std=c++20 --copt="-g" --distdir build //contribs/junk/tests/test_kafka_writer:test_kafka_writer &&
CC=clang bazel build --cxxopt=-std=c++20 --copt="-g" --distdir build //contribs/junk/tests/test_url_freq_map_kafka:test_url_freq_map_kafka &&
CC=clang bazel build --cxxopt=-std=c++20 --copt="-g" --distdir build //contribs/junk/tests/test_url_freq_map:test_url_freq_map &&
CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --distdir build //contribs/junk/tests/test_robotstxt:test_robotstxt &&
CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never --distdir build  //contribs/junk/tests/test_ada:test_ada &&
CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never --distdir build  //contribs/junk/tests/test_url_parsed:test_url_parsed &&
CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never --distdir build  //contribs/junk/tests/test_crawl_attempts:test_crawl_attempts &&
CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never --distdir build  //contribs/junk/tests/test_chrono_ts:test_chrono_ts &&
CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never --distdir build  //contribs/junk/tests/test_urls_days:test_urls_days &&
CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never --distdir build  //contribs/junk/tests/test_url_freq_stats:test_url_freq_stats &&
CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never --distdir build  //contribs/junk/tests/test_urls_days_db:test_urls_days_db &&
CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never --distdir build  //contribs/junk/tests/test_fetch_loop:test_fetch_loop &&
#CC=clang bazel build --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never --distdir build  //src/bin/contentd &&

#CC=clang bazel build --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never --distdir build  //src/adserd &&
#cp bazel-bin/src/adserd/adserd build/bin/
