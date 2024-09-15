source "$(dirname "$0")/common.sh"

# tests fail on old data
#rm data/ds/gened_visits_dataset.json 

set -e

regen="false"
fn="data/ds/gened_visits_dataset.json"

while getopts 'r' flag; do
  case "${flag}" in
    r) regen=true;;
  esac
done

if ! test -f $fn; then
  echo "$fn not exist. Will regenerate.."
  regen=true
fi

if [ "$regen" = true ] ; then
  echo "$fn not exist. Generating.."
  cd infra/local/kafka/ && npm install . ; cd -
  node infra/local/kafka/gen_visits_ds.js file
else
  echo "Will use existing $fn (-r to regenrate or delete it)"
fi

#CC=clang bazel test --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g" --copt -fsanitize=address --copt -DADDRESS_SANITIZER --linkopt -fsanitize=address --strip=never //src/tests

CC=clang bazel test $BAZEL_CACHE_OPT $BAZEL_REPO_CACHE_OPT $BAZEL_REGISTRY_OPT --symlink_prefix $BAZEL_SYMLINK_PREFIX --experimental_enable_bzlmod --cxxopt=-std=c++20 --copt="-g"  --strip=never //src/tests
