# BAZEL Build System

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

######################################################################
# GoogleTest

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest",
    commit = "18f8200e3079b0e54fa00cb7ac55d4c39dcf6da6",
)

######################################################################
# Verilator

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "rules_verilator",
    strip_prefix = "rules_verilator-jim-rc1",
    url = "https://github.com/JimKnowler/rules_verilator/archive/jim-rc1.zip",
)

load(
    "@rules_verilator//verilator:repositories.bzl",
    "rules_verilator_dependencies",
    "rules_verilator_toolchains",
)

rules_verilator_dependencies()
rules_verilator_toolchains()

# Register toolchain dependencies
load("@rules_m4//m4:m4.bzl", "m4_register_toolchains")
m4_register_toolchains()

load("@rules_flex//flex:flex.bzl", "flex_register_toolchains")
flex_register_toolchains()

load("@rules_bison//bison:bison.bzl", "bison_register_toolchains")
bison_register_toolchains()