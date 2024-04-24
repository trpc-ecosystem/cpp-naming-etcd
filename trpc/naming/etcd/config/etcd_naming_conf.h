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

#include <string>
#include <vector>
#include "yaml-cpp/yaml.h"

namespace trpc::naming {

/// @brief etcd plug version
static const char kEtcdPluginName[] = "etcd";
static const char kEtcdSDKVersion[] = "0.0.1";

struct EtcdNodeConfig {
  /// @brief Provider name
  std::string name = "etcd";
  /// @brief Connect endpoint etcd service host
  std::string endpoint = "";
  /// @brief Node prefix
  std::string prefix = "";
  /// @brief Print the configuration
  void Display() const;
};

struct EtcdRegistryConfig {
  std::vector<EtcdNodeConfig> services_config;
  void Display() const;
};

struct EtcdSelectorConfig {
  std::vector<EtcdNodeConfig> services_config;
  void Display() const;
};

struct EtcdNamingConfig {
  EtcdNamingConfig() {}
  explicit EtcdNamingConfig(const std::string& name) { this->name = name; }
  std::string name;
  EtcdRegistryConfig registry_config;
  EtcdSelectorConfig selector_config;
  /// @brief Print the configuration
  void Display() const;
};

}  // namespace trpc::naming

namespace YAML {

template <>
struct convert<trpc::naming::EtcdNodeConfig> {
  static YAML::Node encode(const trpc::naming::EtcdNodeConfig& conf) {
    YAML::Node node;
    node["name"] = conf.name;
    node["endpoint"] = conf.endpoint;
    node["prefix"] = conf.prefix;
    return node;
  }

  static bool decode(const YAML::Node& node, trpc::naming::EtcdNodeConfig& conf) {  // NOLINT
    if (node["name"]) {
      conf.name = node["name"].as<std::string>();
    }
    if (node["endpoint"]) {
      conf.endpoint = node["endpoint"].as<std::string>();
    }
    if (node["prefix"]) {
      conf.prefix = node["prefix"].as<std::string>();
    }
    return true;
  }
};

template <>
struct convert<trpc::naming::EtcdRegistryConfig> {
  static YAML::Node encode(const trpc::naming::EtcdRegistryConfig& config) {
    YAML::Node node;
    node = config.services_config;
    return node;
  }

  static bool decode(const YAML::Node& node, trpc::naming::EtcdRegistryConfig& config) {
    config.services_config = node.as<std::vector<trpc::naming::EtcdNodeConfig>>();
    return true;
  }
};

template <>
struct convert<trpc::naming::EtcdSelectorConfig> {
  static YAML::Node encode(const trpc::naming::EtcdSelectorConfig& config) {
    YAML::Node node;
    node = config.services_config;
    return node;
  }

  static bool decode(const YAML::Node& node, trpc::naming::EtcdSelectorConfig& config) {
    config.services_config = node.as<std::vector<trpc::naming::EtcdNodeConfig>>();
    return true;
  }
};

template <>
struct convert<trpc::naming::EtcdNamingConfig> {
  static YAML::Node encode(const trpc::naming::EtcdNamingConfig& conf) {
    YAML::Node node;
    node["registry"][conf.name] = conf.registry_config;
    node["selector"][conf.name] = conf.selector_config;
    return node;
  }

  static bool decode(const YAML::Node& node, trpc::naming::EtcdNamingConfig& conf) {  // NOLINT
    if (node["registry"] && node["registry"][conf.name]) {
      conf.registry_config = node["registry"][conf.name].as<trpc::naming::EtcdRegistryConfig>();
    }
    if (node["selector"] && node["selector"][conf.name]) {
      conf.selector_config = node["selector"][conf.name].as<trpc::naming::EtcdSelectorConfig>();
    }
    return true;
  }
};

}  // namespace YAML