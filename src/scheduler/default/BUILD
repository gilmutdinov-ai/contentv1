package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "default",
    srcs = [
        "EnqueueLoop.cpp",
        "EnqueueLoopFilter.cpp",
        "EnqueueLoopState.cpp",
        "LoadLoop.cpp",
        "Scheduler.cpp"
    ],
    hdrs = [
        "EnqueueLoop.h",
        "LoadLoop.h",
        "Scheduler.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/scheduler",
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