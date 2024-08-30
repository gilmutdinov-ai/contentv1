
cc_library(
    name = "ut",
    data = ["contentd_ut.conf"],
    srcs = [
        "ContentdTester.cpp"
    ],
    hdrs = [
        "ContentdTester.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@catch2//:catch2_main",
        "//src/bin/contentd/cli",
        "//src/tests/common"
    ],
    copts = [
        "-Isrc/"
    ]
)