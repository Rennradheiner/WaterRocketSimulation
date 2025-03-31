# BUILD
load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

# To update compile_commands.json:
# bazel build //:refresh_compile_commands
# bazel run :refresh_compile_commands
refresh_compile_commands(
    name = "refresh_compile_commands",
    targets = {
        "//...:all": "",
    },
)

cc_binary(
    name = "sim",
    srcs = ["RocketSimulation.cpp"],
    cxxopts = ["-std:c++20"],
    deps = [":water_rocket_library"],
)

cc_library(
    name = "water_rocket_library",
    srcs = ["Rocket.cpp"],
    hdrs = ["Rocket.hpp"],
    copts = ["-D_USE_MATH_DEFINES"],
    cxxopts = ["-std:c++20"],
    visibility = ["//visibility:public"],
)