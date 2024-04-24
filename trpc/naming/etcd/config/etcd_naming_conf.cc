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

#include "trpc/util/log/logging.h"

namespace trpc::naming {

void EtcdNodeConfig::Display() const {
  TRPC_LOG_DEBUG("--------------------------------");

  TRPC_LOG_DEBUG("name:" << name);
  TRPC_LOG_DEBUG("endpoint:" << endpoint);
  TRPC_LOG_DEBUG("prefix:" << prefix);

  TRPC_LOG_DEBUG("--------------------------------");
}

void EtcdRegistryConfig::Display() const {
  TRPC_LOG_DEBUG("--------------------------------");

  for (const auto& it : services_config) {
    it.Display();
  }

  TRPC_LOG_DEBUG("--------------------------------");
}

void EtcdSelectorConfig::Display() const {
  TRPC_LOG_DEBUG("--------------------------------");

  for (const auto& it : services_config) {
    it.Display();
  }

  TRPC_LOG_DEBUG("--------------------------------");
}

void EtcdNamingConfig::Display() const {
  TRPC_LOG_DEBUG("--------------------------------");

  TRPC_LOG_DEBUG("name:" << name);
  registry_config.Display();
  selector_config.Display();

  TRPC_LOG_DEBUG("--------------------------------");
}

}  // namespace trpc::naming