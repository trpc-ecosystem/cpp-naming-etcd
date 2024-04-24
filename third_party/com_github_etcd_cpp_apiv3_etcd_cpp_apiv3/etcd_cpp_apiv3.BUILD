package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "etcd_cpp_apiv3",
    hdrs = glob(["etcd/**/*.hpp"]),
    srcs = glob(["src/**/*.cpp"]),
    defines = ["WITH_GRPC_CHANNEL_CLASS"],
    deps = [
        "@com_github_microsoft_cpprestsdk//:cpprestsdk",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_protobuf//:protobuf",
        "//proto:all_cc_grpc",
    ],
)