#!/bin/bash
source "$(dirname "$0")/common.sh"

set -e

#bazel test //src/tests

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/001_chrono

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/002_ada

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/003_url_parsed

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/004_url_freq_map

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/005_url_freq_stats

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/008_urls_days_db

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/009_host_db

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/010_url_db

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/011_enqueue_loop

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/012_misc

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/013_merge_queue

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/014_url_freq_stats_lf

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/015_crawled_db

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/016_scheduler

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/017_load_loop

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/018_contentd --test_timeout 30

#--copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/019_ch_writer

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/020_fetch_loop

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/021_kafka_sim

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/022_page_db --test_timeout 30

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/023_query_loop

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/024_aho_corasick

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/025_query_db

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/026_api_v1

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/016_scheduler

CC=clang bazel test $BAZEL_CACHE_OPT $BAZEL_REPO_CACHE_OPT $BAZEL_REGISTRY_OPT --symlink_prefix $BAZEL_SYMLINK_PREFIX --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests/027_scheduler_server
