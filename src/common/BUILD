cc_library(
  name = "io-common",
  srcs = ['socket/socket.cc'],
  hdrs = glob([
    "err.h",
    "socket/socket.h",
  ]),
  linkopts = ['-pthread'],
  visibility = ["//visibility:public"],
  linkstatic = 1,
)
