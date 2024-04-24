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

#include "trpc/naming/etcd/etcd_registry.h"

#include <pthread.h>
#include <stdint.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "yaml-cpp/yaml.h"

#include "trpc/common/trpc_plugin.h"
#include "trpc/naming/registry.h"
#include "trpc/naming/registry_factory.h"

#include "trpc/naming/etcd/testing/etcd_testing.h"

namespace trpc::testing {

// There is no Instanceid in the configuration file
class EtcdRegistryWithoutInstanceIdTest : public ::testing::Test {
 protected:
  void SetUp() {
    InitEtcdRegistryWithoutInstanceId();
  }

  void TearDown() {
    trpc::RegistryFactory::GetInstance()->Get("etcd")->Destroy();
    // Destruction again, return directly
    trpc::RegistryFactory::GetInstance()->Get("etcd")->Destroy();
    registry_ = nullptr;
  }

  void InitEtcdRegistryWithoutInstanceId() {
    YAML::Node root = YAML::Load(buildEtcdNamingConfig());
    trpc::naming::EtcdNamingConfig naming_config("etcd");
    YAML::convert<trpc::naming::EtcdNamingConfig>::decode(root, naming_config);
    naming_config.Display();

    trpc::RefPtr<trpc::EtcdRegistry> p = MakeRefCounted<trpc::EtcdRegistry>();
    trpc::RegistryFactory::GetInstance()->Register(p);
    registry_ = trpc::RegistryFactory::GetInstance()->Get("etcd");
    p->SetPluginConfig(naming_config);
    EXPECT_EQ(p.get(), registry_.get());
    // Before initialization, the call method returns directly to fail
    RegistryInfo register_info;
    ASSERT_EQ(-1, registry_->Register(&register_info));
    ASSERT_EQ(-1, registry_->Unregister(&register_info));
    ASSERT_EQ(-1, registry_->HeartBeat(&register_info));
    ASSERT_EQ(0, registry_->Init());
    // Initialize again, return to success directly
    ASSERT_EQ(0, registry_->Init());
    EXPECT_TRUE("" != p->Version());
  }

 protected:
  trpc::RegistryPtr registry_;
};

TEST_F(EtcdRegistryWithoutInstanceIdTest, Register) {
  std::string callee_service = "test1.service";
  trpc::RegistryInfo register_info;
  register_info.host = "127.0.0.1";
  register_info.port = 10001;
  register_info.name = "etcd";

  // In the empty parameters, it fails directly
  trpc::RegistryInfo* register_info_no_exist = nullptr;
  int ret = registry_->Register(register_info_no_exist);
  EXPECT_EQ(-1, ret);
  // No information about the name in register config
  register_info.name = "test.service.no";
  ret = registry_->Register(&register_info);
  EXPECT_EQ(-1, ret);
  // There is information about the name in register config
  register_info.name = callee_service;
  register_info.meta.insert(std::pair<std::string, std::string>("instance_id", "return_instance"));
  ret = registry_->Register(&register_info);
  ASSERT_EQ(0, ret);
  // Duplicate registration, it fails directly
  ret = registry_->Register(&register_info);
  ASSERT_EQ(-1, ret);
  // Unregister about the name in register config
  ret = registry_->Unregister(&register_info);
  ASSERT_EQ(0, ret);
}
}  // namespace trpc::testing

class ETCDTestEnvironment : public testing::Environment {
 public:
  virtual void SetUp() { trpc::TrpcPlugin::GetInstance()->RegisterPlugins(); }
  virtual void TearDown() { trpc::TrpcPlugin::GetInstance()->UnregisterPlugins(); }
};

int main(int argc, char** argv) {
  testing::AddGlobalTestEnvironment(new ETCDTestEnvironment);
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
