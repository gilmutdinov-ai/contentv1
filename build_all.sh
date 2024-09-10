
set -e

source "$(dirname "$0")/build/common.sh"

# run in dev container build/run_shell_docker.sh
#  built with build/docker_build_dev_image.sh

build/run_all_tests.sh
build/build_contentd.sh
build/build_kafka_loader.sh
build/build_sched.sh
build/build_workd.sh
