#ifndef __BpGraph_hpp__
#define __BpGraph_hpp__
#include <vector>
#include <unordered_map>

#include "BpLink.hpp"
#include "BpVariable.hpp"
#include "BpObj.hpp"

namespace bp {

class BpGraph : public BpObj
{
	typedef std::unordered_map<std::string, std::shared_ptr<BpObj>> event_node_map_t;
	typedef std::vector<std::shared_ptr<BpObj>>                     node_vec_t;
	typedef std::unordered_map<std::string, BpVariable>             variable_map_t;
public:
	friend class Bp;
public:
	BpGraph(std::shared_ptr<BpGraph> parent = nullptr);
	BpGraph(std::string name, BpNodeType t, std::shared_ptr<BpGraph> parent = nullptr);

	virtual void Run() override;
	virtual void Logic() {
		// run "input node"
		_input_node.lock()->Run();
	}

	event_node_map_t& GetEvNodes() { return _event_nodes; }
	bool AddEventNode(std::shared_ptr<BpObj>);
	void DelEventNode(std::string name);

	node_vec_t& GetNodes() { return _nodes; }
	std::shared_ptr<BpObj> GetNode(int id);
	void AddNode(std::shared_ptr<BpObj>);
	void DelNode(std::shared_ptr<BpObj>);

	std::vector<BpLink> SearchLinks(int id);
	std::vector<BpLink>& GetLinks() { return _links; }
	int AddLink(BpPin& start_pin, BpPin& end_pin);
	void DelLink(int id);
	
	BpPin* SearchPin(int id);

	/* 获得变量列表 */
	const variable_map_t& GetVariables() { return _vars; }

	/* 获得变量 */
	BpVariable& GetVariable(const std::string& name);
	
	/* 添加变量 */
	bool AddVariable(const std::string& name, const BpVariable& v);

	/* 修改自定义变量名字 */
	bool ModifyVariableName(const std::string& old_name, const std::string& new_name);

	/* 删除一个变量 */
	void RemoveVariable(const std::string& name);

	/* 清空图 */
	void Clear();

	int GetNextID();
	
private:
	void SetNextID(int id);

	void RunEvent(std::string ev);

	void ClearFlag();

	int                         _next_id;
	/* 图的起点 */
	event_node_map_t            _event_nodes;
	/* 图中所有节点列表 */
	node_vec_t                  _nodes;
	/* 图中所有连线 */
    std::vector<BpLink>         _links;
	/* 图中的成员变量 */
	variable_map_t              _vars;
	BpVariable                  _null_val;

	std::weak_ptr<BpObj>        _input_node;
};

} // namespace bp
#endif