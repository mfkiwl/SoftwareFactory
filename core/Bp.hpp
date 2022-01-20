#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <jsoncpp/json/json.h>

#include "BpVariable.hpp"
#include "BpNode.hpp"

namespace bp {

enum class LoadSaveState : int {
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

	LoadSaveState LoadGraph(const std::string& bp_json_path, std::shared_ptr<BpGraph>& g);
	LoadSaveState LoadGraph(const Json::Value& root, std::shared_ptr<BpGraph>& g);

	LoadSaveState SaveGraph(const std::string& bp_json_path, const std::shared_ptr<BpGraph>& g);
	LoadSaveState SaveGraph(Json::Value& root, const std::shared_ptr<BpGraph>& g);

	BpVariable CreateVariable(const std::string& type, const std::string& name);
	BpVariable CreateVariable(const std::string& type, const std::string& name, const std::string& value_desc);

	std::shared_ptr<BpNode> SpawnNode(const std::string& node_name, const BpNodeType t = BpNodeType::BP_NODE_NORMAL);
	/* 生成一个新的变量节点并加入graph */
	std::shared_ptr<BpNode> SpawnVarNode(std::shared_ptr<BpGraph>& g, const std::string& var_type, const std::string& var_name, bool is_get = true);
	/* 根据已有的变量生成节点 */
	std::shared_ptr<BpNode> SpawnVarNode(std::shared_ptr<BpGraph>& g, const std::string& var_name, bool is_get = true);

	const std::shared_ptr<BpContents>& GetContents() const;

	std::shared_ptr<BpGraph> CurEditGraph() { return _cur_edit_graph.lock(); }
	void SetCurEditGraph(std::shared_ptr<BpGraph> g) { _cur_edit_graph = g; }
	bool AddEditGraph(const std::string& name, std::shared_ptr<BpGraph> g);
	const std::unordered_map<std::string, std::shared_ptr<BpGraph>>& GetEditGraphs() { return _edit_graphs; }
private:
	Bp();
	~Bp();
    Bp(const Bp &signal);
    const Bp &operator=(const Bp &);

	// 当前被编辑的图
	std::weak_ptr<BpGraph> _cur_edit_graph;
	std::unordered_map<std::string, std::shared_ptr<BpGraph>> _edit_graphs;
	// 组织模块库
	std::unordered_map<std::string, std::shared_ptr<BpGraph>> _graphs;
	// 基础模块库
	std::shared_ptr<BpModLib> _base_mods;
	// 节点库
	std::shared_ptr<BpNodeLib> _nodes_lib;
	// 库目录
	std::shared_ptr<BpContents> _contents;
};

} // nemespace bp
