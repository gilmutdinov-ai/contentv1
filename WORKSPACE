workspace(name = "contentv1_workspace")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "catch2",
    strip_prefix = "Catch2-3.1.1",
    urls = ["https://github.com/catchorg/Catch2/archive/v3.1.1.tar.gz"],
)

# skylib for rules_foreign_cc
http_archive(
    name = "bazel_skylib",
    sha256 = "cd55a062e763b9349921f0f5db8c3933288dc8ba4f76dd9416aac68acee3cb94",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.5.0/bazel-skylib-1.5.0.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.5.0/bazel-skylib-1.5.0.tar.gz",
    ],
)
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
bazel_skylib_workspace()

http_archive(
    name = "rules_foreign_cc",
    sha256 = "4b33d62cf109bcccf286b30ed7121129cc34cf4f4ed9d8a11f38d9108f40ba74",
    strip_prefix = "rules_foreign_cc-0.11.1",
    url = "https://github.com/bazelbuild/rules_foreign_cc/releases/download/0.11.1/rules_foreign_cc-0.11.1.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

load("@rules_foreign_cc//foreign_cc:defs.bzl", "make")

_ALL_CONTENT = """\
filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)
"""

new_local_repository(
    name = "hellomake",
    path = "./contribs/hellomake/",
    build_file_content = _ALL_CONTENT
)

# not works right now TODO fix
'''
new_local_repository(
    name = "cryptopp",
    path = "./contribs/cryptopp/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
make(
    name = "cryptopp",    
    lib_source = glob(["*.h", "*.cpp"]),,
    out_static_libs = ["libcryptopp.a"],
)
"""
)
'''

new_local_repository(
    name = "librdkafka",
    path = "/usr/include/librdkafka/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "librdkafka",
    hdrs = glob(["**/*.h"]),
    linkopts = ["-lrdkafka++"],
    visibility = ["//visibility:public"],
)
"""
)

# scylla from system
new_local_repository(
    name = "scylla-cpp",
    path = "/usr/local/include/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "scylla-cpp",
    hdrs = glob(["**/*.h"]),
    linkopts = [
        "-L/usr/local/lib/x86_64-linux-gnu/",
        "-lscylla-cpp-driver_static",
        "-luv",
        "-lssl",
        "-lcrypto",
        "-lz"
    ],
)
"""
)

# jsoncpp config parsing
new_local_repository(
    name = "jsoncpp",
    path = "/usr/include/jsoncpp/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "jsoncpp",
    hdrs = glob(["**/*.h"]),
    linkopts = ["-ljsoncpp"]
)
"""
)

# simdjson for parsing streams from kafka, etc
new_local_repository(
    name = "simdjson",
    path = "/usr/include/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "simdjson",
    hdrs = glob(["simdjson.h"]),
    includes = ["./"],
    linkopts = ["-lsimdjson"],
    visibility = ["//visibility:public"],
)
"""
)

# url parser ada
new_local_repository(
    name = "ada",
    path = "./contribs/ada/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "ada",
    srcs = glob([
    ]),
    hdrs = glob([
        "include/*.h",
        "include/ada/*.h",
        "src/*.cpp"
    ]),
    includes = [
        "./include/",
        "./include/ada",
        "./src/"
    ],
    copts = [
        "-Iada/"
    ],
    visibility = ["//visibility:public"]
)
"""
)

new_local_repository(
    name = "concurrentqueue",
    path = "./contribs/concurrentqueue/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "concurrentqueue",
    srcs = glob([
    ]),
    hdrs = glob([
        "concurrentqueue.h"
    ]),
    includes = [
        "./"
    ],
    copts = [
    ],
    visibility = ["//visibility:public"]
)
"""
)

new_local_repository(
    name = "date",
    path = "./contribs/date/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "date",
    srcs = glob([
    ]),
    hdrs = glob([
        "include/date/date.h"
    ]),
    includes = [
        "./include/"
    ],
    copts = [
    ],
    visibility = ["//visibility:public"]
)
"""
)

new_local_repository(
    name = "cpp-httplib",
    path = "./contribs/cpp-httplib/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "cpp-httplib",
    srcs = glob([
    ]),
    hdrs = glob([
        "httplib.h"
    ]),
    includes = [],
    copts = [],
    visibility = ["//visibility:public"]
)
"""
)

new_local_repository(
    name = "aho_corasick",
    path = "./contribs/aho_corasick/src/aho_corasick/",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "aho_corasick",
    srcs = glob([
    ]),
    hdrs = glob([
        "aho_corasick.hpp"
    ]),
    includes = [
        "./"
    ],
    copts = [
    ],
    visibility = ["//visibility:public"]
)
"""
)

## PROTOBUF
# rules_cc defines rules for generating C++ code from Protocol Buffers.
http_archive(
    name = "rules_cc",
    sha256 = "35f2fb4ea0b3e61ad64a369de284e4fbbdcdba71836a5555abb5e194cf119509",
    strip_prefix = "rules_cc-624b5d59dfb45672d4239422fa1e3de1822ee110",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_cc/archive/624b5d59dfb45672d4239422fa1e3de1822ee110.tar.gz",
        "https://github.com/bazelbuild/rules_cc/archive/624b5d59dfb45672d4239422fa1e3de1822ee110.tar.gz",
    ],
)

# rules_proto defines abstract rules for building Protocol Buffers.
http_archive(
    name = "rules_proto",
    sha256 = "2490dca4f249b8a9a3ab07bd1ba6eca085aaf8e45a734af92aad0c42d9dc7aaf",
    strip_prefix = "rules_proto-218ffa7dfa5408492dc86c01ee637614f8695c45",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_proto/archive/218ffa7dfa5408492dc86c01ee637614f8695c45.tar.gz",
        "https://github.com/bazelbuild/rules_proto/archive/218ffa7dfa5408492dc86c01ee637614f8695c45.tar.gz",
    ],
)

load("@rules_cc//cc:repositories.bzl", "rules_cc_dependencies")
rules_cc_dependencies()

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()