
cc_test(
    name = "test_pagedb_scylla",
    srcs = ["test_pagedb_scylla.cpp"],
    copts = ["-Isrc/"],
    deps = [
        "@catch2//:catch2_main",
        "//src/scylla/ut"
    ],
    visibility = ["//visibility:public"]
)
