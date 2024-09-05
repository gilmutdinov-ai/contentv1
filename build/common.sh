echo 'common.sh'

BAZEL_SYMLINK_PREFIX=/tmp/contentv1/
BAZEL_REMOTE_CACHE_OPT=""
#BAZEL_REMOTE_CACHE_OPT="--remote_cache=http://"

if [ -e $dir."WORKSPACE" ]
then
    export WORKSPACE_PATH="$dir"
else
    echo "ok"
fi
