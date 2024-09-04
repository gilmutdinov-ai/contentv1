
cc_library(
    name = "cli",
    srcs = [
        "Workd.cpp"
    ],
    hdrs = [
        "Workd.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/worker",
        "//src/scylla",
        "//src/scheduler/rpc:scheduler_server"
    ],
    copts = [
        "-Isrc/"
    ],
    linkopts=["-lboost_program_options"]
)