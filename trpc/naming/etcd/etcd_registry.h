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

#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "trpc/naming/etcd/config/etcd_naming_conf.h"
#include "trpc/naming/registry.h"

namespace trpc {

/// @brief Etcd Service Registration Plug -in
class EtcdRegistry : public Registry {
 public:
  /// @brief The name of the plugin
  std::string Name() const override { return trpc::naming::kEtcdPluginName; }

  /// @brief Plug -in version
  std::string Version() const { return trpc::naming::kEtcdSDKVersion; }

  /// @brief initialization
  int Init() noexcept override;

  /// @brief In the internal implementation of the plug -in,
  /// it needs to be used when the thread needs to be created. You can use this interface uniformly
  void Start() noexcept override {}

  /// @brief When there is a thread in the internal implementation of the plug -in,
  /// the interface of the stop thread needs to be implemented
  void Stop() noexcept override {}

  /// @brief Various resources to destroy specific plug -in
  void Destroy() noexcept override;

  /// @brief Service registration interface
  int Register(const RegistryInfo* info) override;

  /// @brief Service anti -registration interface
  int Unregister(const RegistryInfo* info) override;

  /// @brief Service heartbeat on the reporting interface
  int HeartBeat(const RegistryInfo* info) override;

  /// @brief Service Heartbeat on the interface
  Future<> AsyncHeartBeat(const RegistryInfo* info) override;

  /// @brief Setter function for plugin_config_
  void SetPluginConfig(const naming::EtcdNamingConfig& config) { plugin_config_ = config; }

 private:
  // Get etcd key
  std::string GetEtcdKey(trpc::naming::EtcdNodeConfig& conf, const RegistryInfo* info);

 private:
  bool init_{false};
  naming::EtcdNamingConfig plugin_config_;
  std::map<std::string, trpc::naming::EtcdNodeConfig> services_config_;
};

}  // namespace trpc
