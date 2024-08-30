package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "scylla",
    srcs = [
        "PageDbScylla.cpp",
        "PageDbScyllaDetail.cpp"
    ],
    hdrs = [
        "PageDbScylla.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@scylla-cpp",
        "//src/db",
        "//src/misc"
    ],
    copts = [
        "-Isrc/"
    ]
)