
cc_test(
    name = "024_aho_corasick",
    srcs = ["ut_aho_corasick.cpp"],
    copts = ["-Isrc/"],
    deps = [
        "@catch2//:catch2_main",
        "@aho_corasick//:aho_corasick",
        "//src/core/ut"
    ],
    visibility = ["//visibility:public"]
)
