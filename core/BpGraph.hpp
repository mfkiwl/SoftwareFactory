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

	BpNodeRunState Run() override;

	void Logic() override {
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
	std::shared_ptr<BpNode> GetNextNodeByOutPinID(int pin_id);
	std::shared_ptr<BpNode> GetPreNodeByInPinID(int pin_id);
	
	/// 添加节点对象
	void AddNode(std::shared_ptr<BpNode>);
	void DelNode(std::shared_ptr<BpNode>);
	void DelNode(int id);

	/**
	 * @brief 搜索连线
	 * 
	 * @param id pin id
	 * @return cosnt BpLink 
	 */
	const BpLink* GetLinkByPinID(int pin_id);

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
	BpLink* GetLink(int id);
	
	/**
	 * @brief 搜索pin对象
	 * 
	 * @param id pin id
	 * @return BpPin* 
	 */
	BpPin* GetPin(int id);

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
	void ClearFlag() override;
	void ClearChildrenFlagByOutPinID(int id);

	/// 设置所有节点的位置，主要用于在编辑器中显示
	void SetNodesPos(const Json::Value& desc) { _nodes_pos = desc; }
	const Json::Value& GetNodesPos() { return _nodes_pos; }

	/// 编辑器调用的函数
	void RunNextEventBeign();
	BpNodeRunState RunNextEvent();

	/// 是否处于调试模式
	bool IsDebugMode() { return _debug_mode; }
	
	/**
	 * @brief 启动调试
	 * @note 收集调试信息：\n 
	 * 		断点节点列表（不用采集，可以根据返回状态得知）\n 
	 * 		事件节点列表 \n 
	 * 			收集每个事件节点对应的执行链节点 TODO
	 */
	void StartDebug();

	/**
	 * @brief 继续执行，直到下一个断点返回
	 */
	BpNodeRunState ContinueDebug();

	/**
	 * @brief 结束调试
	 * @note 清理各种调试flag
	 */
	void StopDebug();

	/**
	 * @brief 获得当前执行到的断点对象
	 * 
	 * @return std::shared_ptr<BpNode> 
	 */
	std::shared_ptr<BpNode> GetCurBreakPoint() {
		return _breakpoint_node.expired() ? nullptr : _breakpoint_node.lock();
	}

	/**
	 * @brief 设置图运行到的断点
	 * @note 内部调用，请谨慎使用
	 * @param n 
	 */
	void SetCurBreakpoint(std::shared_ptr<BpNode> n) {
		_breakpoint_node = n;
	}

	const std::vector<int>& GetCurDebugLinksFlow() { return _debug_cur_links_flow; }
	void AddCurDebugLinkFlow(int link_id) { _debug_cur_links_flow.push_back(link_id); }

	/**
	 * @brief 设置/取消所有节点断点
	 * 
	 * @param[in] b 
	 */
	void SetAllBreakpoints(bool b);

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
	void ClearChildrenFlagByOutPinIDHelper(int pin_id, std::shared_ptr<BpNode> node);

	int                         _next_id = 0;
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
	std::weak_ptr<BpNode>       _breakpoint_node;

	Json::Value                 _nodes_pos;
	bool                        _debug_mode = false;
	std::vector<int>            _debug_cur_links_flow;
};

} // namespace bp
#endif