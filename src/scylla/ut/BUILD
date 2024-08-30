package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "ut",
    srcs = [
        "PageDbScyllaTester.cpp",
    ],
    hdrs = [
        "PageDbScyllaTester.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@catch2//:catch2_main",
        "//src/scylla",
        "//src/tests/common"
    ],
    copts = [
        "-Isrc/"
    ]
)