#ifndef __BP_OBJ_HPP__
#define __BP_OBJ_HPP__
#include <memory>
#include <sstream>

#include "BpPin.hpp"

namespace bp {

// 用于控制显示节点样式
enum class BpNodeStyle : int
{
    BP_NONE = 0, // 未知
	BP_BLUPRINT, // 标准类型的节点
	BP_SIMPLE, // 简单类型的节点,不包含title
};

// 节点类型
enum class BpNodeType : int
{
	BP_NONE = 0, // 未知
	BP_NODE_EV, // 事件节点
	BP_NODE_VAR, //变量节点
	BP_NODE_FUNC,  // 函数节点
	BP_NODE_BASE,  // 基础节点
	BP_NODE_USER, // 用户自定义节点
	BP_GRAPH, // 图节点
    BP_GRAPH_EXEC, // 可以执行的图节点
	BP_GRAPH_INPUT,
	BP_GRAPH_OUTPUT,
};

class BpGraph;
class BpNode : public std::enable_shared_from_this<BpNode>
{
public:
	BpNode(const std::string& name, std::shared_ptr<BpGraph> parent = nullptr);
	virtual ~BpNode();

    virtual void Run();
    virtual void Logic() {};
	
	std::shared_ptr<BpGraph> GetParentGraph() { return _parent_graph.lock(); }
	void SetParentGraph(std::shared_ptr<BpGraph> graph);

	BpNodeStyle GetNodeStyle() { return _node_style; }
    BpNodeType GetNodeType() { return _node_type; }
	int GetID() { return _id; }
	std::string& GetName() { return _name; }

	BpPin& AddPin(const std::string& name, BpPinKind k, BpPinType t, const BpVariable& v);
	bool DelPin(int id);
	std::vector<BpPin>& GetPins(BpPinKind k);
	
	/* 设置Pin都是未赋值状态 */
	void ClearFlag();
	void SetNodeType(BpNodeType t) { _node_type = t; }

	std::string Print() {
		std::stringstream ss;
		ss << PrintNodeType(_node_type) << " (" << _name << "#" << _id << ")";
		return ss.str();
	}
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
protected:
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
};

} // namespace bp
#endif