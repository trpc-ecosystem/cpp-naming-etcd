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

#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "etcd/Response.hpp"

#include "trpc/naming/common/common_defs.h"
#include "trpc/naming/etcd/config/etcd_naming_conf.h"
#include "trpc/naming/selector.h"

static const uint32_t kSelectorCacheTimeout = 60;

/// @brief etcd nodes cache
struct etcd_selector_cache {
  uint64_t cache_time_;
  etcd::Response resp;
};

namespace trpc {

/// @brief etcd service discovery plug -in
class EtcdSelector : public Selector {
 public:
  /// @brief The name of the plugin
  std::string Name() const override { return trpc::naming::kEtcdPluginName; }

  /// @brief Plug -in version
  std::string Version() const override { return trpc::naming::kEtcdSDKVersion; }

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

  /// @brief Get the routing interface of a node that is transferred to a node
  int Select(const SelectorInfo* info, TrpcEndpointInfo* endpoint) override;

  /// @brief Asynchronous acquisition of a adjustable node interface
  Future<TrpcEndpointInfo> AsyncSelect(const SelectorInfo* info) override;

  /// @brief Obtain the interface of node routing information according to strategy
  int SelectBatch(const SelectorInfo* info, std::vector<TrpcEndpointInfo>* endpoints) override;

  /// @brief Asynchronously obtain the interface of node routing information according to strategy
  Future<std::vector<TrpcEndpointInfo>> AsyncSelectBatch(const SelectorInfo* info) override;

  /// @brief Report interface on the result
  int ReportInvokeResult(const InvokeResult* result) override;

  /// @brief Set up the route information interface of the service transfer
  int SetEndpoints(const RouterInfo* info) override { return 0; }

  /// @brief Set framework error codes melting white list
  bool SetCircuitBreakWhiteList(const std::vector<int>& framework_retcodes) override;

  /// @brief Setter function for plugin_config_
  void SetPluginConfig(const naming::EtcdNamingConfig& config) { plugin_config_ = config; }

 private:
  // get one endpoint info from resp
  bool ParseEndpointInfo(const ::etcd::Response& resp, int index, TrpcEndpointInfo& endpoint);

  // Get etcd key
  std::string GetEtcdKey(trpc::naming::EtcdNodeConfig& conf);

  // Get the routing interface of nodes
  int SelectImpl(const SelectorInfo* info, std::vector<TrpcEndpointInfo>* endpoints);

 private:
  bool init_{false};
  std::map<std::string, etcd_selector_cache> nodes_cache_;
  naming::EtcdNamingConfig plugin_config_;
};

}  // namespace trpc
