cc_binary(
    name = "test_kafka_reader",
    srcs = ["test_kafka_reader.cpp"],
    deps = [
        "//src/kafkawrap",
        "@simdjson//:simdjson"
    ],
    copts = [
        "-Iexternal/librdkafka",
        "-Isrc/"
    ]
)
