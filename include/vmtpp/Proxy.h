#pragma once

#include <string>
#include <unordered_map>

namespace vmtpp {

class IEntityAccess;

namespace Proxy {

// We're going to try to implement proxies in a way that's distinct from but still roughly
// comparable to the SDK, so it's easy to compare functionality and get a nice reference!

struct Data {
	std::string name;
	std::unordered_map<std::string, std::string> variables;
};

using Function = void(*)(Data&, std::unordered_map<std::string, std::string>&, const IEntityAccess&);

Function add(const std::string& name, Function proxy);

Function get(const std::string& name);

void exec(Data& data, std::unordered_map<std::string, std::string>& vmtVariables, const IEntityAccess& entity);

void remove(const std::string& name);

} // namespace Proxy

} // namespace vmtpp

#define VMTPP_MATERIAL_PROXY(name, proxy) \
	vmtpp::Proxy::Function VMTPP_MATERIAL_PROXY_##name = vmtpp::Proxy::add(#name, proxy)
