package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "scheduler",
    srcs = [
        "SchedulerStub.cpp"
    ],
    hdrs = [
        "IScheduler.h",
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