#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <jsoncpp/json/json.h>

#include "BpVariable.hpp"

namespace bp {

enum class LoadState : int {
	OK = 0,
	ERR_OPEN_FILE,
	ERR_PARSE,
	ERR_VERSION,
	ERR_VAR,
	ERR_SEARCH_PIN,
};

class BpModLib;
class BpGraph;
class BpNodeLib;
class Bp
{
public:
	static Bp& Instance();

	std::string Version() { return "0.1"; }

	LoadState LoadGraph(const std::string& bp_json_path, std::shared_ptr<BpGraph>& g);
	LoadState LoadGraph(const Json::Value& root, std::shared_ptr<BpGraph>& g);

	void SaveGraph(const std::shared_ptr<BpGraph>&, const std::string&);

	BpVariable CreateVariable(const std::string& type, const std::string& name);
	BpVariable CreateVariable(const std::string& type, const std::string& name, const std::string& value_desc);

	std::shared_ptr<BpModLib> GetBaseLib() { return _base_mods; }

private:
	Bp();
	~Bp();
    Bp(const Bp &signal);
    const Bp &operator=(const Bp &);

	// 组织模块库
	std::unordered_map<std::string, std::shared_ptr<BpGraph>> _graphs;
	// 基础模块库
	std::shared_ptr<BpModLib> _base_mods;
	// 节点库
	std::shared_ptr<BpNodeLib> _nodes_lib;
};

} // nemespace bp
