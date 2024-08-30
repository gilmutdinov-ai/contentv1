cc_library(
    name = "v1",
    srcs = [
        "APIv1Wrap.cpp",
        "APIv1.cpp",
        "APIv1Crypto.cpp",
        "APIv1Parsing.cpp",
        "APIv1HdlToken.cpp",
        "APIv1HdlVersion.cpp",
        "APIv1HdlQG.cpp",
        "APIv1HdlHits.cpp"
    ],
    hdrs = [
        "APIv1Wrap.h",
        "APIv1.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/core",
        "//src/misc",
        "//src/worker",
        
        "@cpp-httplib//:cpp-httplib",
        #"@cryptopp//:cryptopp"
    ],
    copts = ["-Isrc"]
)