cc_library(
    name = "ut",
    data = [],
    srcs = [
        "FetchLoopTester.cpp",
        "QueryLoopTester.cpp"
    ],
    hdrs = [
        "FetchLoopTester.h",
        "QueryLoopTester.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@catch2//:catch2_main",
        "//src/worker",
        "//src/tests/common",
        "//data/ds"
    ],
    copts = [
        "-Isrc/"
    ]
)