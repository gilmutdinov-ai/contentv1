package(
    default_visibility = ["//visibility:public"]
)

cc_library(
    name = "core",
    srcs = [
        "Types.cpp",
        "UrlVisit.cpp",
        "UrlTpStat.cpp",
        "UrlFreqMap.cpp",
        "LastPerMaps.cpp",
        "UrlFreqStats.cpp",
        "UrlFreqStatsLF.cpp",
        "UrlParsed.cpp",
        "HostFreq.cpp",
        "TpSlidWin.cpp",
        "CrawlLimits.cpp",
        "CrawlAttempts.cpp",
        "CrawlTargets.cpp",
        "Robots.cpp",
        "GFQRecord.cpp",
        "FetchResult.cpp",
        "FetchStats.cpp",
        "QueryGroup.cpp",
        "QueryGroupMatcher.cpp"
    ],
    hdrs = [
        "Types.h",
        "UrlVisit.h",
        "UrlTpStat.h",
        "UrlFreqMap.h",
        "LastPerMaps.h",
        "UrlFreqStats.h",
        "UrlFreqStatsLF.h",
        "HostFreq.h",
        "Robots.h",
        "UrlParsed.h",
        "TpSlidWin.h",
        "CrawlLimits.h",
        "CrawlTargets.h",
        "CrawlAttempts.h",
        "GFQRecord.h",
        "FetchResult.h",
        "FetchStats.h",
        "QueryGroup.h",
        "QueryGroupMatcher.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//contribs/robotstxt:robots",
        "@ada//:ada",
        "@simdjson//:simdjson",
        "@aho_corasick//:aho_corasick",
        "//src/misc",
        "//src/proto:proto"
    ],
    copts = [
        "-Isrc/",
        "-Icontribs/"
    ]
)
