package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "scheduler",
    srcs = [
        "EnqueueLoop.cpp",
        "EnqueueLoopFilter.cpp",
        "EnqueueLoopState.cpp",
        "LoadLoop.cpp",
        "Scheduler.cpp",
        "SchedulerStub.cpp"
    ],
    hdrs = [
        "EnqueueLoop.h",
        "LoadLoop.h",
        "Scheduler.h",
        "SchedulerApi.h",
        "SchedulerStub.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/core",
        "//src/ldb",
        "//src/misc",
        
        "//src/kafkawrap",
        "//src/proto",
        "@jsoncpp//:jsoncpp",

        "//src/tests/common"
    ],
    copts = [
        "-Isrc/"
    ]
)