package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "ut",
    srcs = [
        "CrawledDbTester.cpp",
        "PageDbTester.cpp",
        "QueryDbTester.cpp"
    ],
    hdrs = [
        "CrawledDbTester.h",
        "PageDbTester.h",
        "QueryDbTester.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@catch2//:catch2_main",
        "//src/ldb",
        "//src/tests/common"
    ],
    copts = [
        "-Isrc/"
    ]
)