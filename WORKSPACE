workspace(
    name = "com_woa_git_open_source_cpp_naming_etcd",
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

git_repository(
    name = "trpc_cpp",
    remote = "https://github.com/trpc-group/trpc-cpp.git",
    branch = "main",
)

load("@trpc_cpp//trpc:workspace.bzl", "trpc_workspace")
trpc_workspace()

load("//trpc:workspace.bzl", "naming_etcd_workspace")
naming_etcd_workspace()


load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
 
grpc_deps()
 
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
 
grpc_extra_deps()
