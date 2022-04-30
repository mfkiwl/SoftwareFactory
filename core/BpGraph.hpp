#ifndef __BpGraph_hpp__
#define __BpGraph_hpp__
#include <vector>
#include <unordered_map>

#include "BpLink.hpp"
#include "BpVariable.hpp"
#include "BpNode.hpp"
#include "evnode/BpNodeEv.hpp"

namespace bp {

class BpGraph : public BpNode
{
	typedef std::set<std::shared_ptr<BpNodeEv>, BpNodeEvCmp>            event_node_run_t;
	typedef std::unordered_map<std::string, std::shared_ptr<BpNodeEv>>  event_node_map_t;
	typedef std::vector<std::shared_ptr<BpNode>>                        node_vec_t;
	typedef std::unordered_map<std::string, BpVariable>                 variable_map_t;

public:
	friend class Bp;

public:
	BpGraph(std::shared_ptr<BpGraph> parent = nullptr);
	BpGraph(std::string name, BpNodeType t, std::shared_ptr<BpGraph> parent = nullptr);

	virtual void Run() override;

	virtual void Logic() {
		if (_node_type == BpNodeType::BP_GRAPH && !_input_node.expired()){
			_input_node.lock()->Run();
		}
	}

	/**
	 * @brief 添加图节点pin
	 * 
	 * @param name pin名称
	 * @param nt 节点类型，[ BP_GRAPH_INPUT | BP_GRAPH_OUTPUT ]
	 * @param v 变量对象
	 * @note 添加图对象输入输出pin
	 * @return true 
	 * @return false 
	 */
	bool AddModGraphPin(const std::string& name, BpNodeType nt, const BpVariable& v);
	bool DelModGraphPin(int id);

	/**
	 * @brief 获得事件节点对象列表
	 * 
	 * @return event_node_map_t& 
	 */
	event_node_map_t& GetEvNodes() { return _event_nodes; }
	bool AddEventNode(std::shared_ptr<BpNode>);
	void DelEventNode(std::string name);

	/// 获得普通节点列表
	node_vec_t& GetNodes() { return _nodes; }
	/// 获得普通节点
	std::shared_ptr<BpNode> GetNode(int id);
	/// 添加节点对象
	void AddNode(std::shared_ptr<BpNode>);
	void DelNode(std::shared_ptr<BpNode>);
	void DelNode(int id);
	/**
	 * @brief 搜索连线
	 * 
	 * @param id pin id
	 * @return std::vector<BpLink> 
	 */
	std::vector<BpLink> SearchLinks(int id);
	/// 获得图对象中的所有连线
	std::vector<BpLink>& GetLinks() { return _links; }
	int AddLink(BpPin& start_pin, BpPin& end_pin);
	void DelLink(int id);
	/**
	 * @brief 获得连线对象
	 * 
	 * @param id link id
	 * @return BpLink 
	 */
	BpLink GetLink(int id);
	
	/**
	 * @brief 搜索pin对象
	 * 
	 * @param id pin id
	 * @return BpPin* 
	 */
	BpPin* SearchPin(int id);

	/// 获得变量列表
	const variable_map_t& GetVariables() { return _vars; }

	/// 获得变量
	BpVariable& GetVariable(const std::string& name);
	
	/// 添加变量
	bool AddVariable(const std::string& name, const BpVariable& v);

	/// 修改自定义变量名字
	bool ModifyVariableName(const std::string& old_name, const std::string& new_name);

	/// 删除一个变量
	void RemoveVariable(const std::string& name);

	/// 清空图
	void Clear();
	
	/// 清除节点执行后的各种执行状态标记，主要用于下一次执行
	void ClearFlag();

	/// 设置所有节点的位置，主要用于在编辑器中显示
	void SetNodesPos(const Json::Value& desc) { _nodes_pos = desc; }
	const Json::Value& GetNodesPos() { return _nodes_pos; }

	/// 编辑器调用的函数
	void RunNextEventBeign();
	bool RunNextEvent();

	/**
	 * @brief 获得下一个节点ID
	 * 
	 * @return int 
	 * @note 生成的新节点，pin，link使用该函数赋id值
	 * @warning 内部函数，如果不熟悉程序不要轻易调用该函数
	 */
	int GetNextID();

private:
	void SetNextID(int id);

	int                         _next_id;
	/* 图的起点 */
	event_node_run_t            _event_nodes_run;
	event_node_map_t            _event_nodes;
	/* 图中所有节点列表 */
	node_vec_t                  _nodes;
	/* 图中所有连线 */
    std::vector<BpLink>         _links;
	/* 图中的成员变量 */
	variable_map_t              _vars;
	BpVariable                  _null_val;

	std::weak_ptr<BpNode>       _input_node;
	std::weak_ptr<BpNode>       _output_node;

	Json::Value                 _nodes_pos;
};

} // namespace bp
#endif