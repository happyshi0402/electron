// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_net.h"

#include <string>

#include "gin/handle.h"
#include "net/base/network_change_notifier.h"
#include "services/network/public/cpp/features.h"
#include "shell/browser/api/electron_api_url_loader.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/object_template_builder.h"

#include "shell/common/node_includes.h"

namespace electron {

namespace api {

Net::Net(v8::Isolate* isolate) {
  Init(isolate);
}

Net::~Net() = default;

// static
v8::Local<v8::Value> Net::Create(v8::Isolate* isolate) {
  return gin::CreateHandle(isolate, new Net(isolate)).ToV8();
}

// static
void Net::BuildPrototype(v8::Isolate* isolate,
                         v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(gin::StringToV8(isolate, "Net"));
  gin_helper::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetMethod("quit", &IsOnline);
}

// static
bool Net::IsOnline() {
  return net::NetworkChangeNotifier::GetConnectionType() !=
         net::NetworkChangeNotifier::CONNECTION_NONE;
}

}  // namespace api

}  // namespace electron

namespace {

bool IsValidHeaderName(std::string header_name) {
  return net::HttpUtil::IsValidHeaderName(header_name);
}

bool IsValidHeaderValue(std::string header_value) {
  return net::HttpUtil::IsValidHeaderValue(header_value);
}

using electron::api::Net;
using electron::api::SimpleURLLoaderWrapper;

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();

  gin_helper::Dictionary dict(isolate, exports);
  dict.Set("net", Net::Create(isolate));
  dict.Set("Net",
           Net::GetConstructor(isolate)->GetFunction(context).ToLocalChecked());
  dict.SetMethod("isValidHeaderName", &IsValidHeaderName);
  dict.SetMethod("isValidHeaderValue", &IsValidHeaderValue);
  dict.SetMethod("createURLLoader", &SimpleURLLoaderWrapper::Create);
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_net, Initialize)
