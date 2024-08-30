
cc_library(
    name = "cli",
    srcs = [
        "Contentd.cpp"
    ],
    hdrs = [
        "Contentd.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/scheduler",
        "//src/worker",
        "//src/api/v1"
    ],
    copts = [
        "-Isrc/"
    ],
    linkopts=["-lboost_program_options"]
)