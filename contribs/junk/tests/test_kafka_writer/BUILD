cc_binary(
    name = "test_kafka_writer",
    srcs = ["test_kafka_writer.cpp"],
    deps = [
        "//src/kafkawrap",
        "@simdjson//:simdjson"
    ],
    copts = [
        "-Iexternal/librdkafka",
        "-Isrc/"
    ]
)
