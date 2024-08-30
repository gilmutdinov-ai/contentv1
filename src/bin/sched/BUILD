cc_binary(
    name = "sched",
    srcs = [
        "main.cpp"
    ],
    deps = [
        "//src/bin/sched/cli"
    ],
    copts = [
        "-Isrc/",
        "-Iexternal/jsoncpp"
    ],
    linkopts = ["-ljsoncpp"],
    visibility = ["//visibility:public"]
)