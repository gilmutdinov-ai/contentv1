package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "ut",
    srcs = [
        "APIv1Tester.cpp"
    ],
    hdrs = [
        "APIv1Tester.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@cpp-httplib//:cpp-httplib",
        "@catch2//:catch2_main",
        "//src/api/v1",
        "//src/tests/common",
        "//src/worker/ut"
    ],
    copts = [
        "-Isrc/"
    ]
)