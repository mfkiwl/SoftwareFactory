#ifndef __BP_OBJ_HPP__
#define __BP_OBJ_HPP__
#include <memory>

#include "BpPin.hpp"

namespace bp {

// 用于控制显示节点样式
enum class BpNodeType : int
{
    BP_NONE, // 未知
	BP_BLUPRINT, // 标准类型的节点
	BP_SIMPLE, // 简单类型的节点,不包含title
};

// 节点类型
enum class BpObjType : int
{
	BP_NONE = 0, // 未知
	BP_NODE_EV, // 事件节点
	BP_NODE_VAR, //变量节点
	BP_NODE_NORMAL,  // 函数节点
	BP_NODE_BASE,  // 基础节点
	BP_GRAPH, // 图节点
    BP_GRAPH_EXEC, // 可以执行的图节点
};

class BpGraph;
class BpObj  : public std::enable_shared_from_this<BpObj>
{
public:
	BpObj(const std::string& name, std::shared_ptr<BpGraph> parent = nullptr);
	virtual ~BpObj();

    virtual void Run();
    virtual void Logic() = 0;
	
	std::shared_ptr<BpGraph> GetParentGraph() { return _parent_graph.lock(); }
	void SetParentGraph(std::shared_ptr<BpGraph> graph);

	BpNodeType GetNodeType() { return _type; }
    BpObjType GetObjType() { return _obj_type; }
	int GetID() { return _id; }
	std::string& GetName() { return _name; }

	BpPin& AddPin(const std::string& name, BpPinKind k, BpPinType t, const BpVariable& v);
	bool DelPin(int id);
	std::vector<BpPin>& GetPins(BpPinKind k);
	
	/* 设置Pin都是未赋值状态 */
	void ClearFlag();

protected:
	std::weak_ptr<BpGraph>      _parent_graph;
	BpObjType                   _obj_type;
	BpNodeType                  _type;
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