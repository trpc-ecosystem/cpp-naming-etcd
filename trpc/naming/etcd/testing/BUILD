licenses(["notice"])

package(default_visibility = ["//visibility:public"])

exports_files([
    "polaris_test.yaml",
])

cc_library(
    name = "etcd_testing_h",
    hdrs = ["etcd_testing.h"],
    deps = [
        "//trpc/naming/etcd/config:etcd_naming_conf",
    ],
)
