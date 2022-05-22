#ifndef __BP_OBJ_HPP__
#define __BP_OBJ_HPP__
#include <memory>
#include <sstream>

#include "BpPin.hpp"

namespace bp {

/// 节点显示样式
enum class BpNodeStyle : int
{
    BP_NONE = 0,   ///< 未知
	BP_BLUPRINT,   ///< 标准类型的节点
	BP_SIMPLE,     ///< 简单类型的节点,不包含title
};

/// 节点类型
enum class BpNodeType : int
{
	BP_NONE = 0,     ///< 未知
	BP_NODE_EV,      ///< 事件节点
	BP_NODE_VAR,     ///< 变量节点
	BP_NODE_FUNC,    ///< 函数节点
	BP_NODE_BASE,    ///< 基础节点
	BP_NODE_USER,    ///< 用户自定义节点
	BP_GRAPH,        ///< 图节点
    BP_GRAPH_EXEC,   ///< 可以执行的图节点
	BP_GRAPH_INPUT,  ///< 图输入节点
	BP_GRAPH_OUTPUT, ///< 图输出节点
};

/// 节点执行状态
enum class BpNodeRunState : int
{
	BP_RUN_OK = 0,          ///< 成功执行一次事件
	BP_RUN_NO_PARENT,       ///< 节点没有所属图错误
	BP_RUN_BREAKPOINT,      ///< 执行到断点
	BP_RUN_FINISH,          ///< 所有事件执行完成
	BP_RUN_LOGIC_OK,        ///< 成功执行logic
	BP_RUN_LOOP_INTERNAL,   ///< 循环节点中间状态
};

class BpGraph;
class BpNode : public std::enable_shared_from_this<BpNode>
{
public:
	BpNode(const std::string& name, std::shared_ptr<BpGraph> parent = nullptr);
	virtual ~BpNode();

	/**
	 * @brief 节点初始化函数
	 */
	virtual void Init() {};

	/**
	 * @brief 节点逻辑执行函数
	 */
    virtual BpNodeRunState Logic() { return BpNodeRunState::BP_RUN_LOGIC_OK; }
	
	/**
	 * @brief 获得该节点所属于的图对象
	 * 
	 * @return std::shared_ptr<BpGraph> 
	 */
	std::shared_ptr<BpGraph> GetParentGraph() { return _parent_graph.lock(); }
	
	/**
	 * @brief 设置该节点属于的图对象
	 * 
	 * @param[in] graph 图对象 
	 */
	void SetParentGraph(std::shared_ptr<BpGraph> graph);

	/**
	 * @brief 获得该节点样式
	 * 
	 * @return BpNodeStyle 节点样式枚举
	 */
	BpNodeStyle GetNodeStyle() { return _node_style; }
	
	/**
	 * @brief 获得节点类型
	 * 
	 * @return BpNodeType 节点类型枚举
	 */
    BpNodeType GetNodeType() { return _node_type; }
	
	/**
	 * @brief 节点ID
	 * 
	 * @return int ID
	 */
	int GetID() { return _id; }
	
	/**
	 * @brief 节点名称
	 * 
	 * @return std::string& 名称
	 */
	std::string& GetName() { return _name; }
	
	/**
	 * @brief 设置节点名称
	 * 
	 * @param[in] name 名称
	 */
	void SetName(const std::string& name) { _name = name; }
	
	/**
	 * @brief 添加pin
	 * 
	 * @param[in] name pin名称
	 * @param[in] k pin输入输出种类
	 * @param[in] t pin类型
	 * @param[in] v 变量
	 * @return BpPin& pin类型
	 */
	BpPin& AddPin(const std::string& name, BpPinKind k, BpPinType t, const BpVariable& v);
	
	/**
	 * @brief 删除pin
	 * 
	 * @param id pin ID
	 * @return true 
	 * @return false 
	 */
	bool DelPin(int id);

	/**
	 * @brief 获得该节点pin
	 * 
	 * @param k 输出/输入
	 * @return std::vector<BpPin>& 
	 */
	std::vector<BpPin>& GetPins(BpPinKind k);
	
	/**
	 * @brief 设置Pin都是未赋值状态
	 * @note 图执行前都会清除pin状态，如果用户不清楚图执行流程最好不要调用该函数
	 */
	virtual void ClearFlag();

	/**
	 * @brief 设置节点类型
	 * 
	 * @param t 节点类型
	 */
	void SetNodeType(BpNodeType t) { _node_type = t; }

	/**
	 * @brief 输出该节点信息
	 * 
	 * @return std::string 
	 */
	std::string Print() {
		std::stringstream ss;
		ss << PrintNodeType(_node_type) << " (" << _name << "#" << _id << ")";
		return ss.str();
	}

	/**
	 * @brief 输出节点类型字符串
	 * 
	 * @param t 节点类型
	 * @return std::string 
	 */
	std::string PrintNodeType(BpNodeType t) {
		static std::vector<std::string> type_str = 
		{
			"BP_NONE",
			"BP_NODE_EV",
			"BP_NODE_VAR",
			"BP_NODE_FUNC",
			"BP_NODE_BASE",
			"BP_NODE_USER",
			"BP_GRAPH",
			"BP_GRAPH_EXEC",
			"BP_GRAPH_INPUT",
			"BP_GRAPH_OUTPUT",
		};
		return type_str[(int)t];
	}
	
	/**
	 * @brief 节点执行函数
	 * @note 用户不需要继承该函数
	 */
	virtual BpNodeRunState Run();

	/// 设置/取消断点
	void SetBreakpoint(bool b);

	/// 判断收否有断点
	bool HasBreakpoint() {
		return _has_breakpoint;
	}

	/// 判断是否需要构建输入pin
	bool NeedBuildInputPin(const BpPin&);

	/// 判断是否是可执行的输出pin
	bool IsRunableOutputPin(const BpPin&);
	
protected:
	void BuildInput(std::shared_ptr<BpGraph>&);
	void BuildOutput(std::shared_ptr<BpGraph>&);

	std::weak_ptr<BpGraph>      _parent_graph;
	BpNodeType                  _node_type;
	BpNodeStyle                 _node_style;
	std::string                 _name;
    int                         _id;
	int                         _tmp_next_id;
    std::vector<BpPin>          _inputs;
    std::vector<BpPin>          _outputs;
	BpPin                       _null_pin;
	std::vector<BpPin>          _null_pins;
	bool                        _has_breakpoint;
};

} // namespace bp
#endif