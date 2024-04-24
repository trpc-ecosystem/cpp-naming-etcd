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

#include "trpc/naming/etcd/etcd_selector.h"

#include <pthread.h>
#include <stdint.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "yaml-cpp/yaml.h"

#include "trpc/naming/selector.h"
#include "trpc/naming/selector_factory.h"
#include "trpc/naming/registry_factory.h"
#include "trpc/util/ref_ptr.h"

#include "trpc/naming/etcd/testing/etcd_testing.h" 
#include "trpc/naming/etcd/etcd_registry.h"

namespace trpc::testing {

// There is no Instanceid in the configuration file
class EtcdSelectorWithoutInstanceIdTest : public ::testing::Test {
 protected:
  void SetUp() {
    InitEtcdSelector();
    InitServiceNormalData();
  }

  void TearDown() {
    trpc::RegistryInfo register_info1;
    register_info1.host = "127.0.0.1";
    register_info1.port = 10001;
    register_info1.name = "test1.service";
    registry_->Unregister(&register_info1);
    trpc::RegistryInfo register_info2;
    register_info2.host = "127.0.0.1";
    register_info2.port = 10002;
    register_info2.name = "test1.service";
    registry_->Unregister(&register_info2);
    trpc::SelectorFactory::GetInstance()->Get("etcd")->Destroy();
    trpc::RegistryFactory::GetInstance()->Get("etcd")->Destroy();
    selector_ = nullptr;
    registry_ = nullptr;
  }

  void InitEtcdSelector() {
    YAML::Node root = YAML::Load(buildEtcdNamingConfig());
    trpc::naming::EtcdNamingConfig naming_config("etcd");
    YAML::convert<trpc::naming::EtcdNamingConfig>::decode(root, naming_config);
    naming_config.Display();

    trpc::RefPtr<trpc::EtcdRegistry> etcd_register = MakeRefCounted<trpc::EtcdRegistry>();
    trpc::RegistryFactory::GetInstance()->Register(etcd_register);
    registry_ = trpc::RegistryFactory::GetInstance()->Get("etcd");
    etcd_register->SetPluginConfig(naming_config);
    EXPECT_EQ(etcd_register.get(), registry_.get());
    ASSERT_EQ(0, registry_->Init());

    trpc::RefPtr<trpc::EtcdSelector> etcd_selector = MakeRefCounted<trpc::EtcdSelector>();
    trpc::SelectorFactory::GetInstance()->Register(etcd_selector);
    selector_ = trpc::SelectorFactory::GetInstance()->Get("etcd");
    etcd_selector->SetPluginConfig(naming_config);
    EXPECT_EQ(etcd_selector.get(), selector_.get());
    ASSERT_EQ(0, selector_->Init());
    EXPECT_TRUE("" != selector_->Version());
  }

  void InitServiceNormalData() {
    std::string callee_service = "test1.service";
    trpc::RegistryInfo register_info1;
    register_info1.host = "127.0.0.1";
    register_info1.port = 10001;
    register_info1.name = callee_service;
    register_info1.meta.insert(std::pair<std::string, std::string>("instance_id", "return_instance1"));
    int ret = registry_->Register(&register_info1);
    ASSERT_EQ(0, ret);

    trpc::RegistryInfo register_info2;
    register_info2.host = "127.0.0.1";
    register_info2.port = 10002;
    register_info2.name = callee_service;
    register_info2.meta.insert(std::pair<std::string, std::string>("instance_id", "return_instance2"));
    ret = registry_->Register(&register_info2);
    ASSERT_EQ(0, ret);
  }

 protected:
  trpc::SelectorPtr selector_;
  trpc::RegistryPtr registry_;
};

TEST_F(EtcdSelectorWithoutInstanceIdTest, SelectNormal) {
  trpc::SelectorInfo selectInfo;
  selectInfo.name = "test1.service";

  trpc::TrpcEndpointInfo endpoint;
  int ret = selector_->Select(&selectInfo, &endpoint);
  ASSERT_EQ(0, ret);
  ASSERT_FALSE(endpoint.meta["instance_id"].empty());

  ret = selector_->Select(&selectInfo, &endpoint);
  ASSERT_EQ(0, ret);
  ASSERT_FALSE(endpoint.meta["instance_id"].empty());

  trpc::InvokeResult result;
  ret = selector_->ReportInvokeResult(&result);
  ASSERT_EQ(0, ret);

  selector_->AsyncSelect(&selectInfo)
    .Then([this](trpc::Future<trpc::TrpcEndpointInfo>&& select_fut) {
      EXPECT_TRUE(select_fut.IsReady());
      auto endpoint = select_fut.GetValue0();
      EXPECT_FALSE(endpoint.meta["instance_id"].empty());
      return trpc::MakeReadyFuture<>();
    })
    .Wait();

  std::vector<trpc::TrpcEndpointInfo> endpoints;
  selectInfo.policy = SelectorPolicy::ALL;
  ret = selector_->SelectBatch(&selectInfo, &endpoints);
  EXPECT_EQ(0, ret);
  EXPECT_EQ(endpoints.size(), 2);

  selector_->AsyncSelectBatch(&selectInfo)
    .Then([this, &selectInfo](trpc::Future<std::vector<TrpcEndpointInfo>>&& select_fut) {
      EXPECT_TRUE(select_fut.IsReady());
      auto endpoints = select_fut.GetValue0();
      // Node 5 with a weight of 0, and the isolation node 6 without returning
      EXPECT_EQ(endpoints.size(), 2);
      return trpc::MakeReadyFuture<>();
    })
    .Wait();
}

}  // namespace trpc::testing

class EtcdTestEnvironment : public testing::Environment {
 public:
  virtual void SetUp() {  }
  virtual void TearDown() { trpc::SelectorFactory::GetInstance()->Clear(); }
};

int main(int argc, char** argv) {
  testing::AddGlobalTestEnvironment(new EtcdTestEnvironment);
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
