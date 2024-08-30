package(
    default_visibility = ["//visibility:public"]
)

# BUILD.bazel
proto_library(
    name = "contentv1_protos",
    srcs = ["CrawlAttempts.proto",
        "UrlDays.proto",
        "UrlsDays.proto",
        "UrlDb.proto",
        "GFQRecord.proto",
        "QueryGroup.proto",
        "FetchResult.proto"],
    deps = ["@com_google_protobuf//:any_proto"],
)

# BUILD.bazel
load("@rules_proto_grpc_cpp//:defs.bzl", "cpp_proto_library")

cpp_proto_library(
    name = "proto",
    protos = [":contentv1_protos"]
)
