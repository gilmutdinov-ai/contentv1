
cc_test(
    name = "001_yaml",
    data = ["ut_conf.yml"],
    srcs = ["ut_yaml.cpp"],
    copts = ["-Isrc/"],
    deps = [
        "@catch2//:catch2_main",
        "@yaml-cpp//:yaml-cpp"
    ],
    visibility = ["//visibility:public"]
)
