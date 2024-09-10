cc_binary(
    name = "kafka_loader",
    srcs = [
        "main.cpp"
    ],
    deps = [
        "//src/scheduler",
        "//src/kafkawrap",
        "//src/misc",
        "//src/tests/common"
    ],
    copts = [
        "-Isrc/",
        "-Iexternal/jsoncpp"
    ],
    linkopts = ["-ljsoncpp"],
    visibility = ["//visibility:public"]
)