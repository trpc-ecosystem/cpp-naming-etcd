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

#include <map>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "etcd/Client.hpp"
#include "rapidjson/document.h"

#include "trpc/codec/trpc/trpc.pb.h"
#include "trpc/common/config/trpc_config.h"
#include "trpc/naming/selector_factory.h"
#include "trpc/naming/etcd/config/etcd_naming_conf.h"
#include "trpc/util/log/logging.h"
#include "trpc/util/string_helper.h"
#include "trpc/util/string_util.h"


namespace trpc {

int EtcdSelector::Init() noexcept {
  if (init_) {
    TRPC_FMT_DEBUG("Already init");
    return 0;
  }
  if (plugin_config_.name.empty()) {
    trpc::naming::EtcdNamingConfig config;
    if (!trpc::TrpcConfig::GetInstance()->GetPluginConfig<trpc::naming::EtcdSelectorConfig>(
            "selector", "etcd", config.selector_config)) {
      TRPC_FMT_ERROR("get selector etcd config error, use default value");
    }
    plugin_config_ = config;
  }
  init_ = true;
  return 0;
}

void EtcdSelector::Destroy() noexcept {
  if (!init_) {
    TRPC_FMT_DEBUG("No init yet");
    return;
  }

  init_ = false;
}

std::string EtcdSelector::GetEtcdKey(trpc::naming::EtcdNodeConfig& conf) {
  if (conf.prefix == "") {
    return "/" + conf.name;
  }
  return "/" + conf.prefix + "_" + conf.name;
}

bool EtcdSelector::ParseEndpointInfo(const etcd::Response& resp, int index, TrpcEndpointInfo& endpoint) {
  const std::string& str_key_info = resp.key(index);
  std::vector<std::string> v_key = trpc::util::SplitString(str_key_info, '/');
  if (v_key.size() < 3) {
    TRPC_FMT_ERROR("Parse failed, service_name:{}, info err", str_key_info);
    return false;
  }

  std::vector<std::string>::iterator it_host = v_key.end() - 2;
  std::vector<std::string>::iterator it_port = v_key.end() - 1;

  endpoint.host = *it_host;
  endpoint.port = trpc::util::Convert<int, std::string>(*it_port);

  std::string str_meta = resp.value(index).as_string();
  rapidjson::Document doc;
  if (!doc.Parse(str_meta.c_str()).HasParseError()) {
    if (doc.HasMember("meta") && doc["meta"].IsObject()) {
      const rapidjson::Value& meta = doc["meta"];
      for (rapidjson::Value::ConstMemberIterator it = meta.MemberBegin(); it != meta.MemberEnd(); ++it) {
        if (meta.HasMember(it->name) && it->value.IsString()) {
          endpoint.meta[it->name.GetString()] = it->value.GetString();
        }
      }
    }
  }

  return true;
}

// Get the routing interface of nodes
int EtcdSelector::SelectImpl(const SelectorInfo* info, std::vector<TrpcEndpointInfo>* endpoints) {
  if (!init_) {
    TRPC_FMT_ERROR("No init yet");
    return -1;
  }

  trpc::naming::EtcdNodeConfig *name_conf = nullptr;
  auto conf = std::any_cast<trpc::naming::EtcdNamingConfig>(plugin_config_);
  for (size_t i = 0; i < conf.selector_config.services_config.size(); i++) {
    if (info->name == conf.selector_config.services_config[i].name) {
      name_conf = &conf.selector_config.services_config[i];
      break;
    }
  }
  if (name_conf == nullptr) {
    TRPC_FMT_ERROR("Select failed, service_name:{}, no service conf info", info->name);
    return -1;
  }
  if (name_conf->endpoint == "") {
    TRPC_FMT_ERROR("Select failed, service_name:{}, no service conf endpoint info", info->name);
    return -1;
  }

  uint64_t cur_time = time(0);
  std::string str_key = GetEtcdKey(*name_conf);
  etcd::Response resp;

  if (nodes_cache_.count(str_key) &&
    nodes_cache_[str_key].cache_time_ + kSelectorCacheTimeout > cur_time) {
      resp = nodes_cache_[str_key].resp;
  } else {
    etcd::Client etcd_client(name_conf->endpoint);
    resp = etcd_client.ls(str_key).get();
  }

  if (0 == resp.error_code()) {
    size_t size = resp.keys().size();
    if (size == 0) {
      TRPC_FMT_ERROR("Select failed, service_name:{}, no service info", str_key);
      return -1;
    }
  } else {
    TRPC_FMT_ERROR("Select failed, sdk returnCode:{}, service_name:{}, err_info:{}",
    resp.error_code(), str_key, resp.error_message());
    return -1;
  }

  nodes_cache_[str_key].cache_time_ =  cur_time;
  nodes_cache_[str_key].resp = resp;

  size_t size = resp.keys().size();
  if (info->policy == SelectorPolicy::ALL) {
    for (size_t index = 0; index < size; index++) {
      TrpcEndpointInfo etcd_response_info;
      if (ParseEndpointInfo(resp, index, etcd_response_info)) {
        endpoints->push_back(std::move(etcd_response_info));
      }
    }
  } else if (info->policy == SelectorPolicy::ONE) {
    int index = random() % size;
    TrpcEndpointInfo etcd_response_info;
    if (ParseEndpointInfo(resp, index, etcd_response_info)) {
      endpoints->push_back(std::move(etcd_response_info));
    }
  } else {
    TRPC_FMT_ERROR("Select failed policy not support, service_name:{},", str_key);
    return -1;
  }

  if (endpoints->size() == 0) {
    TRPC_FMT_ERROR("Select failed not node, service_name:{},", str_key);
    return -1;
  }

  for (auto it = endpoints->begin(); it != endpoints->end(); ++it) {
    TRPC_FMT_DEBUG("Select result {}:{}, service_name:{}", it->host, it->port, str_key);
  }

  return 0;
}


// Get the routing interface of a node that is transferred to a node
int EtcdSelector::Select(const SelectorInfo* info, TrpcEndpointInfo* endpoint) {
  if (!init_) {
    TRPC_FMT_ERROR("No inited yet");
    return -1;
  }
  std::vector<TrpcEndpointInfo> endpoints;
  int ret = SelectImpl(info, &endpoints);
  if (ret == 0) {
    endpoint->host = endpoints[0].host;
    endpoint->port = endpoints[0].port;
    endpoint->meta = endpoints[0].meta;
    TRPC_FMT_DEBUG("Select result {}:{}, id:{}, service_name:{}", endpoint->host, endpoint->port,
                 endpoint->id, info->name);
  }

  return ret;
}

// Asynchronous acquisition of a adjustable node interface
Future<TrpcEndpointInfo> EtcdSelector::AsyncSelect(const SelectorInfo* info) {
  TrpcEndpointInfo endpoint;
  // Only blocked when the first call is called
  int ret = Select(info, &endpoint);
  if (ret != 0) {
    return MakeExceptionFuture<TrpcEndpointInfo>(CommonException("AsyncSelect error"));
  }

  return MakeReadyFuture<TrpcEndpointInfo>(std::move(endpoint));
}

// Obtain the interface of node routing information according to strategy: Support press ALL, default Random
int EtcdSelector::SelectBatch(const SelectorInfo* info, std::vector<TrpcEndpointInfo>* endpoints) {
  if (!init_) {
    TRPC_FMT_ERROR("No init yet");
    return -1;
  }
  return SelectImpl(info, endpoints);
}

Future<std::vector<TrpcEndpointInfo>> EtcdSelector::AsyncSelectBatch(const SelectorInfo* info) {
  std::vector<TrpcEndpointInfo> endpoints;
  // Only blocked when the first call is called
  int ret = SelectBatch(info, &endpoints);
  if (ret != 0) {
    return MakeExceptionFuture<std::vector<TrpcEndpointInfo>>(CommonException("AsyncSelectBatch error"));
  }

  return MakeReadyFuture<std::vector<TrpcEndpointInfo>>(std::move(endpoints));
}

// Report interface on the result
int EtcdSelector::ReportInvokeResult(const InvokeResult* result) {
  if (!init_ || result == nullptr) {
    TRPC_FMT_ERROR("Invalid parameter: invoke result is invalid");
    return -1;
  }

  return 0;
}

bool EtcdSelector::SetCircuitBreakWhiteList(const std::vector<int>& framework_retcodes) {
  return true;
}

}  // namespace trpc
