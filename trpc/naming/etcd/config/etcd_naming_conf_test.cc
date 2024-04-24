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

#include "trpc/naming/etcd/config/etcd_naming_conf.h"

#include <iostream>
#include <memory>
#include <string>

#include "gtest/gtest.h"
#include "yaml-cpp/yaml.h"
#include "trpc/common/config/config_helper.h"

#include "trpc/naming/etcd/testing/etcd_testing.h"

namespace trpc::testing {

TEST(EtcdNamingConfig, Load) {
  std::string naming_config_str = buildEtcdNamingConfig();
  YAML::Node root = YAML::Load(naming_config_str);

  trpc::naming::EtcdNamingConfig naming_conf("etcd");
  ASSERT_TRUE(YAML::convert<trpc::naming::EtcdNamingConfig>::decode(root, naming_conf));
  naming_conf.Display();
}

}  // namespace trpc::testing
