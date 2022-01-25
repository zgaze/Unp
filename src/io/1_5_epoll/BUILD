cc_binary(
  name = "echo_client",
  srcs = ["client.c"],
  deps = [
    "//src/common:io-common",
  ],
)

cc_binary(
  name = "echo_server",
  srcs = ["server.cc"],
  deps = [
    "//src/common:io-common",
  ],
  linkopts = ["-lpthread"],
)
