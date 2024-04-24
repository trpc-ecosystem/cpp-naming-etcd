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

#ifndef BUILD_EXCLUDE_NAMING_POLARIS
#include "trpc/naming/etcd/etcd_registry.h"

#include <utility>

#include "etcd/Client.hpp"
#include "yaml-cpp/yaml.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "trpc/common/config/trpc_config.h"
#include "trpc/naming/registry_factory.h"
#include "trpc/util/log/logging.h"
#include "trpc/util/string/string_util.h"

namespace trpc {

int EtcdRegistry::Init() noexcept {
  if (init_) {
    TRPC_FMT_DEBUG("Already init");
    return 0;
  }
  if (plugin_config_.name.empty()) {
    trpc::naming::EtcdNamingConfig config;
    if (!TrpcConfig::GetInstance()->GetPluginConfig<trpc::naming::EtcdRegistryConfig>("registry", "etcd",
                                                                                  config.registry_config)) {
      TRPC_FMT_ERROR("get registry etcd config error, use default");
    }

    if (!trpc::TrpcConfig::GetInstance()->GetPluginConfig<trpc::naming::EtcdSelectorConfig>(
            "selector", "etcd", config.selector_config)) {
      TRPC_FMT_ERROR("get selector etcd config error, use default value");
    }
    plugin_config_ = config;
  }

  auto config = std::any_cast<const trpc::naming::EtcdNamingConfig>(plugin_config_);
  for (const auto& service_config : config.registry_config.services_config) {
    services_config_.insert(std::make_pair(service_config.name, service_config));
  }

  init_ = true;
  return 0;
}

void EtcdRegistry::Destroy() noexcept {
  if (!init_) {
    TRPC_FMT_DEBUG("No init yet");
    return;
  }

  services_config_.clear();
  init_ = false;
}

std::string EtcdRegistry::GetEtcdKey(trpc::naming::EtcdNodeConfig& conf, const RegistryInfo* info) {
  if (conf.prefix == "") {
    return "/" + conf.name + "/" + info->host + "/" +
      trpc::util::Convert<std::string, int>(info->port);
  }

  return "/" + conf.prefix + "_" + conf.name + "/" + info->host + "/" +
    trpc::util::Convert<std::string, int>(info->port);
}

int EtcdRegistry::Register(const RegistryInfo* info) {
  if (!init_) {
    TRPC_FMT_ERROR("No init yet");
    return -1;
  }

  if (info == nullptr) {
    TRPC_FMT_ERROR("Input parameter is empty");
    return -1;
  }

  if (services_config_.find(info->name) == services_config_.end()) {
    TRPC_FMT_ERROR("Register failed, registry.etcd has no name:{} confif", info->name);
    return -1;
  }

  trpc::naming::EtcdNodeConfig& conf = services_config_[info->name];
  if (conf.endpoint == "") {
    TRPC_FMT_ERROR("Register failed, registry.etcd has no endpoint:{} confif", info->name);
    return -1;
  }

  etcd::Client etcd_client(conf.endpoint);
  std::string str_key = GetEtcdKey(conf, info);
  rapidjson::StringBuffer str_meta_json;
  rapidjson::Writer<rapidjson::StringBuffer> writer(str_meta_json);
  writer.StartObject();
  {
    writer.Key("meta");
    writer.StartObject();
    {
      for (auto& it : info->meta) {
          writer.Key(it.first.c_str());
          writer.String(it.second.c_str());
      }
    }
    writer.EndObject();
  }
  writer.EndObject();

  TRPC_FMT_DEBUG("Register info service_name:{}, value_info:{}", str_key, str_meta_json.GetString());

  etcd::Response resp = etcd_client.add(str_key, str_meta_json.GetString()).get();
  if (0 == resp.error_code()) {
    TRPC_FMT_DEBUG("Register success, service_name:{}, service_info:{}", str_key, str_meta_json.GetString());
    return 0;
  } else {
    TRPC_FMT_ERROR("Register failed, sdk returnCode:{}, service_name:{}, err_info:{}",
    resp.error_code(), str_key, resp.error_message());
  }
  return -1;
}

int EtcdRegistry::Unregister(const RegistryInfo* info) {
  if (!init_) {
    TRPC_FMT_ERROR("No init yet");
    return -1;
  }

  if (info == nullptr) {
    TRPC_FMT_ERROR("Input parameter is empty");
    return -1;
  }

  if (services_config_.find(info->name) == services_config_.end()) {
    TRPC_FMT_ERROR("Unregister failed, registry.etcd has no name:{} confif", info->name);
    return -1;
  }

  trpc::naming::EtcdNodeConfig& conf = services_config_[info->name];
  if (conf.endpoint == "") {
    TRPC_FMT_ERROR("Unregister failed, registry.etcd has no endpoint:{} confif", info->name);
    return -1;
  }

  etcd::Client etcd_client(conf.endpoint);
  std::string str_key = GetEtcdKey(conf, info);
  etcd::Response resp = etcd_client.rm(str_key).get();
  if (resp.is_ok()) {
    TRPC_FMT_DEBUG("Deregister success, service_name:{}", str_key);
    return 0;
  } else {
    TRPC_FMT_ERROR("Deregister failed, sdk returnCode:{}, service_name:{}, err_info:{}",
    resp.error_code(), str_key, resp.prev_value().as_string());
  }
  return -1;
}

int EtcdRegistry::HeartBeat(const RegistryInfo* info) {
  TRPC_FMT_DEBUG("HeartBeat Start...");
  if (!init_) {
    TRPC_FMT_ERROR("No init yet");
    return -1;
  }

  if (info == nullptr) {
    TRPC_FMT_ERROR("Input parameter is empty");
    return -1;
  }
  return 0;
}

Future<> EtcdRegistry::AsyncHeartBeat(const RegistryInfo* info) {
  TRPC_FMT_DEBUG("AsyncHeartBeat Start...");
  if (!init_) {
    std::string error_str("No init yet");
    TRPC_LOG_ERROR(error_str);
    return MakeExceptionFuture<>(CommonException(error_str.c_str()));
  }

  if (info == nullptr) {
    std::string error_str("Input parameter is empty");
    TRPC_LOG_ERROR(error_str);
    return MakeExceptionFuture<>(CommonException(error_str.c_str()));
  }

  return trpc::MakeReadyFuture<>();
}

}  // namespace trpc
#endif
