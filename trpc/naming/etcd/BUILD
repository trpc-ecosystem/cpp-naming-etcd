# Description: trpc-cpp.

licenses(["notice"])
   
package(default_visibility = ["//visibility:public"])


cc_library(
    name = "etcd_selector",
    srcs = ["etcd_selector.cc"],
    hdrs = ["etcd_selector.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//trpc/naming/etcd/config:etcd_naming_conf",
        "@trpc_cpp//trpc/codec/trpc:trpc",
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/util/log:logging",
        "@trpc_cpp//trpc/naming:selector",
        "@trpc_cpp//trpc/naming:selector_factory",
        "@trpc_cpp//trpc/util:string_util",
        "@trpc_cpp//trpc/util:string_helper",
        "@com_github_jbeder_yaml_cpp//:yaml-cpp",
        "@com_github_etcd_cpp_apiv3_etcd_cpp_apiv3//:etcd_cpp_apiv3",
        "@com_github_rapidjson//:rapidjson"
    ],
)

cc_test(
    name = "etcd_selector_test",
    srcs = ["etcd_selector_test.cc"],
    deps = [
        "//trpc/naming/etcd:etcd_selector",
        "//trpc/naming/etcd:etcd_registry",
        "//trpc/naming/etcd/testing:etcd_testing_h",
        "@trpc_cpp//trpc/naming:selector",
        "@trpc_cpp//trpc/naming:selector_factory",
        "@trpc_cpp//trpc/naming:registry_factory",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
        "@com_github_jbeder_yaml_cpp//:yaml-cpp",
        "@com_github_etcd_cpp_apiv3_etcd_cpp_apiv3//:etcd_cpp_apiv3",
    ],
)

cc_library(
    name = "etcd_registry",
    srcs = ["etcd_registry.cc"],
    hdrs = ["etcd_registry.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//trpc/naming/etcd/config:etcd_naming_conf",
        "@trpc_cpp//trpc/naming:registry",
        "@trpc_cpp//trpc/naming:registry_factory",
        "@trpc_cpp//trpc/util/log:logging",
        "@trpc_cpp//trpc/util/string:string_util",
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/filter:filter_manager",
        "@com_github_jbeder_yaml_cpp//:yaml-cpp",
        "@com_github_etcd_cpp_apiv3_etcd_cpp_apiv3//:etcd_cpp_apiv3",
        "@com_github_rapidjson//:rapidjson"
    ]
)

cc_test(
    name = "etcd_registry_test",
    srcs = ["etcd_registry_test.cc"],
    deps = [
        "//trpc/naming/etcd:etcd_registry",
        "//trpc/naming/etcd/testing:etcd_testing_h",
        "@trpc_cpp//trpc/common:trpc_plugin",
        "@trpc_cpp//trpc/naming:registry",
        "@trpc_cpp//trpc/naming:registry_factory",
        "@com_google_googletest//:gtest",
        "@com_github_jbeder_yaml_cpp//:yaml-cpp",
        "@com_github_etcd_cpp_apiv3_etcd_cpp_apiv3//:etcd_cpp_apiv3",
    ],
)


cc_library(
    name = "etcd_selector_filter",
    srcs = [],
    hdrs = ["etcd_selector_filter.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/filter",
        "@trpc_cpp//trpc/naming:selector_workflow",
    ],
)

cc_library(
    name = "etcd_selector_api",
    srcs = ["etcd_selector_api.cc"],
    hdrs = ["etcd_selector_api.h"],
    deps = [
        "//trpc/naming/etcd:etcd_selector",
        "//trpc/naming/etcd:etcd_selector_filter",
        "@trpc_cpp//trpc/common:trpc_plugin",
        "@trpc_cpp//trpc/naming:selector_factory",
    ],
)

cc_library(
    name = "etcd_registry_api",
    srcs = ["etcd_registry_api.cc"],
    hdrs = ["etcd_registry_api.h"],
    deps = [
        "//trpc/naming/etcd:etcd_registry",
        "@trpc_cpp//trpc/common:trpc_plugin",
    ],
)
