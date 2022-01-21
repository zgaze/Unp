workspace(name = "io-rpc")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

git_repository(
    name = "gtest",
    remote = "https://github.com/google/googletest",
    branch = "v1.10.x",
)

git_repository(
  name = "bazel_tool",
  remote = "https://github.com/bazelbuild/bazel.git",
  tag = "4.2.1"
)
