cc_binary(
    name = "workd",
    srcs = [
        "main.cpp"
    ],
    deps = [
        "//src/bin/workd/cli"
    ],
    copts = [
        "-Isrc/",
        "-Iexternal/jsoncpp"
    ],
    linkopts = ["-ljsoncpp"],
    visibility = ["//visibility:public"]
)