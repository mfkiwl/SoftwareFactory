#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

#include <jsoncpp/json/json.h>

#include "BpVariable.hpp"
#include "BpNode.hpp"

namespace bp {

/// 加载保存返回状态
enum class LoadSaveState : int {
	OK = 0,
	ERR_OPEN_FILE,
	ERR_PARSE,
	ERR_VERSION,
	ERR_VAR,
	ERR_CREATE_NODE,
	ERR_SEARCH_PIN,
	ERR_JSON_FMT,
};

class BpModLib;
class BpGraph;
class BpNodeLib;
class BpGraphModLib;
class Bp
{
public:
	/**
	 * @brief 蓝图管理单例
	 * 
	 * @return Bp&  单例对象
	 */
	static Bp& Instance();

	/**
	 * @brief 用户注册自定义模块
	 * @note 需要提供库目录和节点创建函数
	 * @return true 
	 * @return false 
	 */
	bool RegisterUserMod(std::shared_ptr<BpContents>, 
			std::function<std::shared_ptr<BpNode>(const std::string&)>);

	/**
	 * @brief 加载蓝图
	 * 
	 * @param[in] bp_json_path 蓝图文件
	 * @param[out] g 蓝图对象
	 * @param[in] desc 节点位置等信息
	 * @return LoadSaveState 
	 */
	LoadSaveState LoadGraph(const std::string& bp_json_path, std::shared_ptr<BpGraph>& g, Json::Value& desc);
	/**
	 * @brief 加载蓝图
	 * 
	 * @param[in] root 加载到内存的蓝图文件
	 * @param[in] graph_name 蓝图对象名称
	 * @param[out] g 蓝图对象
	 * @param[in] desc 节点位置等信息
	 * @return LoadSaveState 
	 */
	LoadSaveState LoadGraph(const Json::Value& root, const std::string& graph_name, std::shared_ptr<BpGraph>& g, Json::Value& desc);
	/**
	 * @brief 保存蓝图对象到文件
	 * 
	 * @param[in] bp_json_path 要保存的文件路径
	 * @param[in] g 蓝图对象
	 * @param[in] desc 节点位置等信息
	 * @return LoadSaveState 
	 */
	LoadSaveState SaveGraph(const std::string& bp_json_path, const std::shared_ptr<BpGraph>& g, const Json::Value& desc);
	/**
	 * @brief 保存蓝图对象到Json:Value
	 * 
	 * @param[in] root 存储到Json:Value
	 * @param[in] g 蓝图对象
	 * @param[in] desc 节点位置等信息
	 * @return LoadSaveState 
	 */
	LoadSaveState SaveGraph(Json::Value& root, const std::shared_ptr<BpGraph>& g, const Json::Value& desc);

	/**
	 * @brief 创建变量
	 * 
	 * @param[in] type 变量类型 
	 * @return BpVariable 
	 */
	BpVariable CreateVariable(const std::string& type);
	/**
	 * @brief 创建变量
	 * 
	 * @param[in] type 变量类型 
	 * @param[in] value_desc 变量默认值，使用json描述
	 * @return BpVariable 
	 */
	BpVariable CreateVariable(const std::string& type, const std::string& value_desc);

	/**
	 * @brief 生成非变量节点
	 * 
	 * @param[in] node_name 节点名称
	 * @param[in] t 节点类型,见 BpNodeType
	 * @return std::shared_ptr<BpNode> 
	 */
	std::shared_ptr<BpNode> SpawnNode(const std::string& node_name, const BpNodeType t = BpNodeType::BP_NODE_FUNC);
	/**
	 * @brief 创建变量节点
	 * @param[in] g 蓝图对象
	 * @param[in] var_type 变量类型
	 * @param[in] var_name 变量名称
	 * @param[in] is_get 
	 * 		true 获得变量节点 \n 
	 * 		false 设置变量节点
	 * @note 创建新变量并创建变量节点
	 * @return std::shared_ptr<BpNode> 
	 */
	std::shared_ptr<BpNode> SpawnVarNode(std::shared_ptr<BpGraph>& g, const std::string& var_type, const std::string& var_name, bool is_get = true);
	/**
	 * @brief 创建变量节点
	 * @param[in] g 蓝图对象
	 * @param[in] var_name 变量名称
	 * @param[in] is_get 
	 * 		true 获得变量节点 \n 
	 * 		false 设置变量节点
	 * @note 为已有变量创建变量节点
	 * @return std::shared_ptr<BpNode> 
	 */
	std::shared_ptr<BpNode> SpawnVarNode(std::shared_ptr<BpGraph>& g, const std::string& var_name, bool is_get = true);

	/**
	 * @brief 获得变量类型对应的颜色
	 * 
	 * @param var_type 变量类型
	 * @return uint32_t A[24:31],B[16:23],G[8:15],R[0:7]
	 */
	uint32_t GetVarColor(const std::string& var_type);

	/**
	 * @brief 获得模块库目录
	 * 
	 * @return const std::shared_ptr<BpContents>& 
	 */
	const std::shared_ptr<BpContents>& GetContents() const;

	/* 设置当前编辑图 */
	std::shared_ptr<BpGraph> CurEditGraph() { return _cur_edit_graph.lock(); }
	void SetCurEditGraph(std::shared_ptr<BpGraph> g) { _cur_edit_graph = g; }
	void SetCurEditGraph(const std::string& name) { 
		if (_edit_graphs.find(name) != _edit_graphs.end()) {
			_cur_edit_graph = _edit_graphs[name];
			return;
		}
		LOG(ERROR) << "can't find current graph " << name;
	}
	bool AddEditGraph(const std::string& name, std::shared_ptr<BpGraph> g);
	bool HasEditGraph(const std::string& name) { return _edit_graphs.find(name) != _edit_graphs.end(); }
	const std::unordered_map<std::string, std::shared_ptr<BpGraph>>& GetEditGraphs() { return _edit_graphs; }
private:
	Bp();
	~Bp();
    Bp(const Bp &signal);
    const Bp &operator=(const Bp &);

	std::vector<int> Version(const std::string& path);

	LoadSaveState LoadGraph(const Json::Value& root, const Json::Value& json_graph, std::shared_ptr<BpGraph>& g, Json::Value& desc);

	/// 当前被编辑的图
	std::weak_ptr<BpGraph> _cur_edit_graph;
	std::unordered_map<std::string, std::shared_ptr<BpGraph>> _edit_graphs;
	/// 组织模块库
	std::shared_ptr<BpGraphModLib> _graph_mods;
	/// 基础模块库
	std::shared_ptr<BpModLib> _base_mods;
	/// 节点库
	std::shared_ptr<BpNodeLib> _nodes_lib;
	/// 库目录
	std::shared_ptr<BpContents> _contents;
	std::vector<int> _version;
	/// 用户注册的模块库
	std::unordered_map<std::string, std::function<std::shared_ptr<BpNode>(const std::string&)>> _user_spawn_nodes;
};

} // nemespace bp
