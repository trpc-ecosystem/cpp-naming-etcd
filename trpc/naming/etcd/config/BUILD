# Description: trpc-cpp.

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "etcd_naming_conf",
    srcs = ["etcd_naming_conf.cc"],
    hdrs = ["etcd_naming_conf.h"],
    deps = [
        "@com_github_jbeder_yaml_cpp//:yaml-cpp",
        "@trpc_cpp//trpc/common/config:config_helper",
        "@trpc_cpp//trpc/util/log:logging",
    ],
)

cc_test(
    name = "etcd_naming_conf_test",
    srcs = ["etcd_naming_conf_test.cc"],
    deps = [
        ":etcd_naming_conf",
	"//trpc/naming/etcd/testing:etcd_testing_h",
        "@com_google_googletest//:gtest_main",
    ],
)
