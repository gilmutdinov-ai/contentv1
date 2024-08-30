
cc_library(
    name = "cli",
    srcs = [
        "Sched.cpp"
    ],
    hdrs = [
        "Sched.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/scheduler/rpc:scheduler_server",
        "//src/api/v1",
        "//src/scylla"
    ],
    copts = [
        "-Isrc/"
    ],
    linkopts=["-lboost_program_options"]
)