//
//
// Tencent is pleased to support the open source community by making tRPC available.
//
// Copyright (C) 2024 THL A29 Limited, a Tencent company.
// All rights reserved.
//
// If you have downloaded a copy of the tRPC source code from Tencent,
// please note that tRPC source code is licensed under the  Apache 2.0 License,
// A copy of the Apache 2.0 License is included in this file.
//
//

#include <memory>
#include <string>

#include "fmt/format.h"
#include "trpc/common/trpc_app.h"
#include "trpc/naming/registry_factory.h"

#include "examples/server/greeter_service.h"
#include "trpc/naming/etcd/etcd_registry.h"
#include "trpc/naming/etcd/etcd_registry_api.h"
#include "trpc/naming/etcd/etcd_selector_api.h"


namespace test::helloworld {

class HelloWorldServer : public ::trpc::TrpcApp {
 public:
  int Initialize() override {
    TRPC_FMT_INFO(" HelloWorldServer Initialize end---------");
    const auto& config = ::trpc::TrpcConfig::GetInstance()->GetServerConfig();
    // Set the service name, which must be the same as the value of the `/server/service/name` configuration item
    // in the configuration file, otherwise the framework cannot receive requests normally.
    std::string service_name = fmt::format("{}.{}.{}.{}", "trpc", config.app, config.server, "Greeter");
    TRPC_FMT_INFO("service name:{}", service_name);
    RegisterService(service_name, std::make_shared<GreeterServiceImpl>());

    // If the trpc-cpp self-registration and heartbeat report are not enabled,
    // you need to manually register and report the heartbeat
    register_info_.name = service_name;
    register_info_.host = config.services_config[0].ip;
    register_info_.port = config.services_config[0].port;
    register_info_.meta["namespace"] = trpc::TrpcConfig::GetInstance()->GetGlobalConfig().env_namespace;

    // If the config enable_self_register is false, you need to manually register
    // RegisterEtcd();
    return 0;
  }

  void RegisterEtcd() {
    TRPC_FMT_INFO(" RegisterEtcd Initialize end---------");
    auto registry = trpc::static_pointer_cast<trpc::EtcdRegistry>(trpc::RegistryFactory::GetInstance()->Get("etcd"));
    auto ret = registry->Register(&register_info_);
    if (ret != 0) {
      TRPC_FMT_ERROR("Failed to register service");
    } else {
      TRPC_FMT_INFO("Success to register service");
    }
  }

  void Destroy() override {}

 private:
  trpc::RegistryInfo register_info_;
};

}  // namespace test::helloworld

int main(int argc, char** argv) {
  test::helloworld::HelloWorldServer helloworld_server;

  ::trpc::etcd::registry::Init();
  ::trpc::etcd::selector::Init();

  helloworld_server.Main(argc, argv);
  helloworld_server.Wait();

  return 0;
}
