package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "ldb",
    srcs = [
        "HostDb.cpp",
        "UrlDb.cpp",
        "UrlDbLoad.cpp",
        "UrlDbSnapThread.cpp",
        "UrlDbPolicy.cpp",
        "UrlDbPolicyRobots.cpp",
        "UrlDbModify.cpp",
        "UrlsDays.cpp",
        "UrlsDaysDb.cpp",
        "UrlsDaysDbSnap.cpp",
        "CrawledDb.cpp",
        "PageDb.cpp",
        "QueryDb.cpp"
    ],
    hdrs = [
        "HostDb.h",
        "UrlDb.h",
        "UrlsDays.h",
        "UrlsDaysDb.h",
        "CrawledDb.h",
        "PageDb.h",
        "QueryDb.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/db",
        "//src/core",
        "//src/misc",
        "//src/proto",
        "@ada//:ada",
        "@concurrentqueue//:concurrentqueue"
    ],
    copts = [
        "-Isrc/"
    ]
)