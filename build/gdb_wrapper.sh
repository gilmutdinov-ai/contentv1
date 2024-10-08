#!/usr/bin/env bash
#!/bin/bash
source "$(dirname "$0")/common.sh"

# We need to run gdb ... < /dev/tty in order to get stdin as an interactive process under bazel run's --run_under
# Additionally, we get dropped into the runfiles of the target directory. This is suboptimal, so we use sed to find
# the execroot, and tell gdb to cd there so we get both our source files and our external sources.
gdb -ex "directory $(pwd | sed -E 's|(.*/execroot/contentv1_workspace/).*|\1|')" "$@" < /dev/stdin > /dev/stdour 2>/dev/stderr
