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
	BP_NODE_NORMAL,  // 标准节点
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
	void SetParentGraph(std::shared_ptr<BpGraph> graph) { _parent_graph = graph; }

	virtual BpNodeType GetNodeType() { return _type; }
    virtual BpObjType GetObjType() { return _obj_type; }
	virtual int GetID() { return _id; }
	virtual std::string& GetName() { return _name; }

	virtual BpPin& AddPin(const std::string& name, BpPinKind k, BpPinType t, const BpVariable& v);
	virtual bool DelPin(int id);
	virtual std::vector<BpPin>& GetPins(BpPinKind k);
	
	/* 设置Pin都是未赋值状态 */
	void ClearFlag();

protected:
	std::weak_ptr<BpGraph>      _parent_graph;
	BpObjType                   _obj_type;
	BpNodeType                  _type;
	std::string                 _name;
    int                         _id;
    std::vector<BpPin>          _inputs;
    std::vector<BpPin>          _outputs;
	BpPin                       _null_pin;
	std::vector<BpPin>          _null_pins;
};

} // namespace bp
#endif