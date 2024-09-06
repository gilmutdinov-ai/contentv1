echo 'common.sh'

BAZEL_SYMLINK_PREFIX=/tmp/contentv1/
BAZEL_CACHE_MOUNT_OPT="-v bazel_cache:/root/.cache/bazel"
BAZEL_CACHE_OPT="--disk_cache=/root/.cache/bazel"

BAZEL_REGISTRY_URL="https://github.com/gilmutdinov-ai/bazel-central-registry.git"

BAZEL_MIRROR_DIR="/root/bazel_mirror"
BAZEL_MIRROR_MOUNT_OPT="-v bazel_mirror:$BAZEL_MIRROR_DIR"
BAZEL_REPO_CACHE_OPT="--repository_cache=$BAZEL_MIRROR_DIR/repo_cache"
BAZEL_REGISTRY_OPT="--registry=file://$BAZEL_MIRROR_DIR/bazel-central-registry"

if [ -e $dir."WORKSPACE" ]
then
    export WORKSPACE_PATH="$dir"
else
    echo "ok"
fi
