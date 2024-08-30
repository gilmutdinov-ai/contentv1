#!/bin/bash
set -e

# run in dev container build/run_shell_docker.sh
#  built with build/docker_build_dev_image.sh

build/run_all_tests.sh
build/build_contentd.sh
