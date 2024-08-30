package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "kafkawrap",
    srcs = [
        "UrlVisitsReader.cpp",
        "GFQReader.cpp"
    ],
    hdrs = [
        "UrlVisitsReader.h",
        "GFQReader.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/misc/kafkawrap",
        "//src/core"
    ],
    copts = [
        "-Isrc/"
    ]
)