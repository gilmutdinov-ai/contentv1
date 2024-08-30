package(
    default_visibility = ["//visibility:public"]
)

# BUILD.bazel
proto_library(
    name = "scheduler_proto",
    srcs = ["scheduler.proto"],
    deps = ["@com_google_protobuf//:any_proto", "//src/proto:contentv1_protos"]
)

# BUILD.bazel
load("@rules_proto_grpc_cpp//:defs.bzl", "cpp_grpc_library")

cpp_grpc_library(
    name = "scheduler_grpc",
    protos = [":scheduler_proto"],
    deps = ["//src/proto:proto"]
)

cc_library(
    name = "scheduler_server",
    srcs = [
        "SchedulerServer.cpp",
        "SchedulerServerCallData.cpp",
        "SchedulerServerCallDataMethods.cpp",
        "SchedulerServerCallDataTryFetchUrls.cpp",
        "SchedulerServerCallDataOnFetched.cpp",
        "SchedulerClient.cpp"
    ],
    hdrs = [
        "SchedulerServer.h",
        "SchedulerServerCallData.h",
        "SchedulerServerCallDataMethods.h",
        "SchedulerServerCallDataTryFetchUrls.h",
        "SchedulerServerCallDataOnFetched.h",
        "SchedulerClient.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//src/scheduler",
        "//src/scheduler/rpc:scheduler_grpc",
    ],
    copts = ["-Isrc/"]
)