[中文](./README.zh_CN.md)

[TOC]

# Introduction

[Etcd](https://etcd.io/)  is a highly available key-value storage system for shared configuration and service discovery. It is developed by CoreOS for distributed system configuration management and service discovery. To facilitate users to connect to the Etcd system, we provide the Etcd data source plugin, which allows users to easily load configuration data from etcd clusters and use this data in their programs.

# Usage

For detailed usage examples, please refer to: [Etcd examples](./examples/)。

## Dependency

### Bazel

1. Import the repository

   In the WORKSPACE file of the project, import the cpp-naming-etcd repository and its dependencies:
    ```
    load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

    git_repository(
        name = "trpc_cpp",
        remote = "https://github.com/trpc-group/trpc-cpp.git",
        branch = "main",
    )
   
    load("@trpc_cpp//trpc:workspace.bzl", "trpc_workspace")
    trpc_workspace()
    
    git_repository(
        name = "cpp-naming-etcd",
        remote = "https://github.com/trpc-ecosystem/cpp-naming-etcd.git",
        branch = "main",
    )
   
    load("@cpp-naming-etcd//trpc:workspace.bzl", "naming_etcd_workspace")
    naming_etcd_workspace()
    ```

   [etcd-client-cpp](https://github.com/etcd-cpp-apiv3/etcd-cpp-apiv3).

2. Import the plugin

   In the target that needs to use Etcd, introduce the "trpc/naming/etcd:etcd_registry_api" dependency. For example:
    ```
   cc_binary(
       name = "helloworld_server",
       srcs = ["helloworld_server.cc"],
       deps = [
           "@cpp-naming-etcd//trpc/naming/etcd:etcd_registry_api",
           ...
       ],
   )
    ```

### Cmake

Not supported yet.

## Plugin Configuration

To use the Etcd plugin, you must add the plugin configuration in the framework configuration file:
```yaml
plugins:
  registry:
    etcd: 
      - name:  trpc.test.helloworld.Greeter
        prefix: ""
        endpoint: http://127.0.0.1:2379
```

The following is a detailed explanation of these configuration items:

| 配置项   | 类型   | 默认值               | 说明                                                         |
| -------- | ------ | -------------------- | ------------------------------------------------------------ |
| name     | string | No default value, must be set   | The name of the etcd registry, used in the code to specify which etcd registry to use through the ::trpc::naming::Register function |
| endpoint | string | No default value, must be set   | The access address of the etcd cluster, in the format "http://ip:port"                |
| prefix   | string | Empty string (optional configuration) | The prefix of the key that stores configuration data in etcd, used to distinguish different configuration data       |

## Function support

The current etcd name service Registry is supported, but heartbeat reporting is not supported
the etcd name serves Selector, SelectorPolicy::ONE random route and SelectorPolicy::ALL are supported. Other routing policies and circuit breakers are not supported

## Initialization

The cpp-etcd-naming plugin provides the plugin registration interface ::trpc::naming::etcd::Init(), which users need to call before the framework starts.

For the server scenario, the user needs to call it in the TrpcApp::Initialize function when the service starts:

    ```cpp
    #include "trpc/naming/etcd/etcd_registry_api.h"
    
    class HelloworldServer : public ::trpc::TrpcApp {
     public:
      ...
      void RegisterEtcd() override {
        auto registry = trpc::static_pointer_cast<trpc::EtcdRegistry>(trpc::RegistryFactory::GetInstance()->Get("etcd"));
        auto ret = registry->Register(&register_info);
      }
    };
    ```

2. For the pure client scenario, it needs to be called after the framework configuration is started and before the other modules of the framework are started:

    ```cpp
    #include "trpc/naming/etcd/etcd_selector_api.h"

    int main(int argc, char* argv[]) {
        ParseClientConfig(argc, argv);
        ::trpc::etcd::selector::Init();
        return ::trpc::RunInTrpcRuntime([]() { return Run(); });
    }
    ```

## Precautions

* Before using the cpp-etcd-naming plugin, you need to ensure that the etcd cluster is installed and configured

# Install etcd cluster
1. Visit the official etcd [GitHub](https://github.com/etcd-io/etcd) repository, on the page, you can find detailed guidance on installing and deploying an etcd cluster. You can select an installation method based on your operating system and environment.

2. For simple testing and development purposes, you can run a single-node etcd cluster locally. Please refer to the "Getting Started" section in the etcd official documentation to learn how to start an etcd instance locally.

3. For production environments, it is recommended to deploy a multi-node etcd cluster to ensure high availability and data consistency. Please refer to the "Runtime Configuration" section in the etcd official documentation to learn how to configure and deploy a multi-node etcd cluster.

# Use etcdctl tool to set key-value pairs

* etcdctl is a command-line client tool for etcd, used to interact with the etcd cluster. You can use the etcdctl tool to set, get, delete, and monitor key-value pairs. Please make sure you have installed etcdctl correctly, for the specific installation method, please refer to the etcd official documentation.
* The basic syntax for using etcdctl to set key-value pairs is as follows:
    ```shell
    etcdctl put <key> <value>
    ```
  For example, to set a key called "salutation_config.json" with the value being the configuration data, you can execute the following command:
    ```shell
    etcdctl put salutation_config.json '{"hello": "world"}'
    ```
* The basic syntax for using etcdctl to get key-value pairs is as follows:
    ```shell
    etcdctl get <key>
    ```
  For example, to get the value of the key named "salutation_config.json", you can execute the following command:
    ```shell
    etcdctl get salutation_config.json
    ```
* For more information on how to use the etcdctl tool, see [Etcd](https://etcd.io/docs/) In official documents [Interacting with etcd](https://etcd.io/docs/v3.5/dev-guide/interacting_v3/) parts.