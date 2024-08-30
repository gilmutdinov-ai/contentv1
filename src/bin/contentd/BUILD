cc_binary(
    name = "contentd",
    srcs = [
        "main.cpp"
    ],
    deps = [
        "//src/bin/contentd/cli"
    ],
    copts = [
        "-Isrc/",
        "-Iexternal/jsoncpp"
    ],
    linkopts = ["-ljsoncpp"],
    visibility = ["//visibility:public"]
)