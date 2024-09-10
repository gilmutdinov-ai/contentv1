source "$(dirname "$0")/common.sh"

rm -rf $BAZEL_MIRROR_DIR/*

#git clone $BAZEL_REGISTRY_URL $BAZEL_MIRROR_DIR/bcr
#cd $BAZEL_MIRROR_DIR/bcr
#git reset --hard 94f4df1f0d33aae6b96920594f808d4ee2b75935
#cd -

bazel sync $BAZEL_REPO_CACHE_OPT $BAZEL_REGISTRY_OPT
bazel fetch $BAZEL_REPO_CACHE_OPT $BAZEL_REGISTRY_OPT
