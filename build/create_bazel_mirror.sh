source "$(dirname "$0")/common.sh"

rm -rf $BAZEL_MIRROR_DIR/*

cd $BAZEL_MIRROR_DIR
git clone $BAZEL_REGISTRY_URL
cd -

bazel sync $BAZEL_REPO_CACHE_OPT $BAZEL_REGISTRY_OPT
bazel fetch $BAZEL_REPO_CACHE_OPT $BAZEL_REGISTRY_OPT
