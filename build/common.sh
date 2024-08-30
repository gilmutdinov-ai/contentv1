echo 'common.sh'

BAZEL_SYMLINK_PREFIX=/tmp/contentv1/

if [ -e $dir."WORKSPACE" ]
then
    export WORKSPACE_PATH="$dir"
else
    echo "ok"
fi
