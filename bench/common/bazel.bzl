load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_zig//zig:defs.bzl", "zig_binary")

def bench(name):
    cc_binary(
        name = name,
        srcs = [
            name + ".cpp",
        ],
        deps = [
            "//common:common",
            "@google_benchmark//:benchmark_main",
        ],
    )

def zig_bench(name):
    zig_binary(
        name = "zig." + name,
        main = name + ".zig",
    )
