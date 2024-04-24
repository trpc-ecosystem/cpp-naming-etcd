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

#pragma once

#include "trpc/naming/etcd/config/etcd_naming_conf.h"

namespace trpc::testing {
inline std::string buildEtcdNamingConfig() {
  trpc::naming::EtcdNamingConfig naming_config("etcd");
  trpc::naming::EtcdNodeConfig node1_config;
  node1_config.name = "test1.service";
  node1_config.prefix = "test1";
  node1_config.endpoint = "http://127.0.0.1:2379";

  trpc::naming::EtcdNodeConfig node2_config;
  node2_config.name = "test2.service";
  node2_config.prefix = "test2";
  node2_config.endpoint = "test2";
  // registry config
  naming_config.registry_config.services_config.push_back(node1_config);
  naming_config.registry_config.services_config.push_back(node2_config);

  // select config
  naming_config.selector_config.services_config.push_back(node1_config);
  naming_config.selector_config.services_config.push_back(node2_config);

  YAML::Node naming_config_node(naming_config);
  std::stringstream strstream;
  strstream << naming_config_node;
  return strstream.str();
}
}  // namespace trpc::testing

