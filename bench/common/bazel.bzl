def bench(name):
    native.cc_binary(
        name = name,
        srcs = [
            name + ".cpp",
        ],
        deps = [
            "//common:common",
            "@google_benchmark//:benchmark_main",
        ]
    )
