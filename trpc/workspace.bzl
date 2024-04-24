"""This module contains some dependency"""

# buildifier: disable=load
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def clean_dep(dep):
    return str(Label(dep))

# buildifier: disable=function-docstring-args
def naming_etcd_workspace(path_prefix = "", repo_name = "", **kwargs):
    """Build rules for the trpc project

    Note: The main idea is to determine the required version of dependent software during the build process
          by passing in parameters.
    Args:
        path_prefix: Path prefix.
        repo_name: Repository name of the dependency.
        kwargs: Keyword arguments, dictionary type, mainly used to specify the version and sha256 value of
                dependent software, where the key of the keyword is constructed by the `name + version`.
                eg: protobuf_ver,zlib_ver...
    Example:
        trpc_workspace(path_prefix="", repo_name="", protobuf_ver="xxx", protobuf_sha256="xxx", ...)
        Here, `xxx` is the specific specified version. If the version is not specified through the key,
        the default value will be used. eg: protobuf_ver = kwargs.get("protobuf_ver", "3.8.0")
    """

    if not native.existing_rule("com_googlesource_code_re2"):
        http_archive(
            name = "com_googlesource_code_re2",
            sha256 = "0915741f524ad87debb9eb0429fe6016772a1569e21dc6d492039562308fcb0f",
            strip_prefix = "re2-2020-10-01",
            urls = ["https://github.com/google/re2/archive/2020-10-01.tar.gz"],
        )

    new_git_repository(
        name = "com_github_etcd_cpp_apiv3_etcd_cpp_apiv3",
        remote = "https://github.com/ChenG0207/etcd-cpp-apiv3.git",
        branch = "master",
        build_file = "//third_party/com_github_etcd_cpp_apiv3_etcd_cpp_apiv3:etcd_cpp_apiv3.BUILD",
    )

    new_git_repository(
        name = "com_github_microsoft_cpprestsdk",
        remote = "https://github.com/microsoft/cpprestsdk.git",
        branch = "master",
        build_file = "//third_party/com_github_microsoft_cpprestsdk:cpprestsdk.BUILD",
    )
    
    http_archive(
        name = "com_github_grpc_grpc",
        strip_prefix = "grpc-1.45.0",
        sha256 = "ec19657a677d49af59aa806ec299c070c882986c9fcc022b1c22c2a3caf01bcd",
        urls = ["https://github.com/grpc/grpc/archive/refs/tags/v1.45.0.tar.gz"],
    )

    http_archive(
        name = "com_github_rapidjson",
        urls = [
            "https://github.com/Tencent/rapidjson/archive/v1.1.0.tar.gz",
        ],
        sha256 = "bf7ced29704a1e696fbccf2a2b4ea068e7774fa37f6d7dd4039d0787f8bed98e",
        strip_prefix = "rapidjson-1.1.0",
        build_file = "//:third_party/com_github_rapidjson/rapidjson.BUILD",
    )