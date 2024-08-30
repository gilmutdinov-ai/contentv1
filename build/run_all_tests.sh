#!/bin/bash
source "$(dirname "$0")/common.sh"

# tests fail on old data
rm data/ds/gened_visits_dataset.json 

set -e

if ! test -f data/ds/gened_visits_dataset.json; then
  echo "data/ds/gened_visits_dataset.json not exist. Generating.."
  node infra/local/kafka/gen_visits_ds.js file
fi

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests
CC=clang bazel test --symlink_prefix $BAZEL_SYMLINK_PREFIX --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/001_chrono

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/002_ada

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/003_url_parsed

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/009_host_db

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/010_url_db

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/011_enqueue_loop

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/012_enq_db

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests/013_merge_queue
