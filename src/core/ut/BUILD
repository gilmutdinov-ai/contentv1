package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "ut",
    srcs = [
        "QueryGroupMatcherTester.cpp"
    ],
    hdrs = [
        "UrlFreqStatsTester.h",
        "QueryGroupMatcherTester.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@catch2//:catch2_main",
        "//src/core",
        "//src/tests/common"
    ],
    copts = [
        "-Isrc/"
    ]
)