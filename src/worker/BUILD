package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "worker",
    srcs = [
        "Worker.cpp",
        "FetchLoop.cpp",
        "QueryLoop.cpp",
        "ReqBatch.cpp"
    ],
    hdrs = [
        "Worker.h",
        "FetchLoop.h",
        "QueryLoop.h",
        "ReqBatch.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/scheduler",
        "//src/misc/httpcli",
        "@jsoncpp//:jsoncpp"
    ],
    copts = [
        "-Isrc/",
        "-Iexternal/jsoncpp"
    ]
)