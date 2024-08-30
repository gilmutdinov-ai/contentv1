package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "ut",
    data = ["scheduler_ut.conf", "scheduler_srv_ut.conf", "load_loop_ut.conf"],
    srcs = [
        "EnqueueLoopTester.cpp",
        "SchedulerTester.cpp",
        "SchedulerServerTester.cpp",
        "LoadLoopTester.cpp"
    ],
    hdrs = [
        "EnqueueLoopTester.h",
        "SchedulerTester.h",
        "SchedulerServerTester.h",
        "LoadLoopTester.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@catch2//:catch2_main",
        "//src/scheduler",
        "//src/scheduler/rpc:scheduler_server",
        "//src/tests/common",
        "//data/ds"
    ],
    copts = [
        "-Isrc/"
    ]
)