#include <fstream>

#include "Bp.hpp"
#include "BpGraph.hpp"
#include "BpPin.hpp"
#include "BpLink.hpp"

namespace bp {

BpGraph::BpGraph(std::shared_ptr<BpGraph> parent) 
	: BpNode("", parent)
	, _next_id(0) {}

BpGraph::BpGraph(std::string name, BpNodeType t, std::shared_ptr<BpGraph> parent)
	: BpNode(name, parent)
	, _next_id(0) {
	_node_type = t;
}

void BpGraph::Clear() {
	_next_id = 0;
	_links.clear();
	_vars.clear();
	_nodes.clear();
	_event_nodes.clear();
}

int BpGraph::GetNextID() {
	return ++_next_id;
}

void BpGraph::SetNextID(int id) {
	// 从配置文件加载蓝图时会用到该函数设置下次node的id
	// 其他情况不要调用该函数
	_next_id = id - 1;
}

bool BpGraph::AddModGraphPin(const std::string& name, BpNodeType nt, const BpVariable& v) {
	if (nt == BpNodeType::BP_GRAPH_INPUT && !_input_node.expired()) {
		auto in_node = _input_node.lock();
		auto graph_node_pin_id = AddPin(name, BpPinKind::BP_INPUT, BpPinType::BP_VALUE, v).ID;
		in_node->AddPin(name, BpPinKind::BP_OUTPUT, BpPinType::BP_VALUE, v).SetGraphPinID(graph_node_pin_id);
	} else if (nt == BpNodeType::BP_GRAPH_OUTPUT && !_output_node.expired()) {
		auto out_node = _output_node.lock();
		auto graph_node_pin_id = AddPin(name, BpPinKind::BP_OUTPUT, BpPinType::BP_VALUE, v).ID;
		out_node->AddPin(name, BpPinKind::BP_INPUT, BpPinType::BP_VALUE, v).SetGraphPinID(graph_node_pin_id);
	} else {
		return false;
	}
	return true;
}

bool BpGraph::DelModGraphPin(int id) {
	auto node = GetNode(id);
	// FIXME: 删除pin时，如果有连线那么连线状态未重置
	if (node->GetNodeType() == BpNodeType::BP_GRAPH_INPUT) {
		auto sz = node->GetPins(BpPinKind::BP_OUTPUT).size();
		if (sz > 1) {
			auto pin = node->GetPins(BpPinKind::BP_OUTPUT)[sz - 1];
			auto graph_pin_id = pin.GetGraphPinID();
			auto pin_id = pin.ID;
			node->DelPin(pin_id);
			DelPin(graph_pin_id);
		}
	} else if (node->GetNodeType() == BpNodeType::BP_GRAPH_OUTPUT) {
		auto sz = node->GetPins(BpPinKind::BP_INPUT).size();
		if (sz > 1) {
			auto pin = node->GetPins(BpPinKind::BP_INPUT)[sz - 1];
			auto graph_pin_id = pin.GetGraphPinID();
			auto pin_id = pin.ID;
			node->DelPin(pin_id);
			DelPin(graph_pin_id);
		}
	} else {
		return false;
	}
	return true;
}

BpVariable& BpGraph::GetVariable(const std::string& name) {
	if (_vars.find(name) == _vars.end()) {
		LOG(ERROR) << "can't find var " << name;
		return _null_val;
	}
	return _vars[name];
}

bool BpGraph::AddVariable(const std::string& name, const BpVariable& v) {
	if (_vars.find(name) == _vars.end()) {
		_vars[name] = v;
		return true;
	}
	LOG(ERROR) << "create var " << name << "failed";
	return false;
}

bool BpGraph::ModifyVariableName(const std::string& old_name, const std::string& new_name) {
	if (_vars.find(old_name) == _vars.end()) return false;
	if (_vars.find(new_name) != _vars.end()) return false;

	_vars[new_name] = _vars[old_name];
	_vars.erase(old_name);
	return true;
}

void BpGraph::RemoveVariable(const std::string& name) {
	if (_vars.find(name) == _vars.end()) return;
	_vars.erase(name);
}

void BpGraph::AddNode(std::shared_ptr<BpNode> node) {
	if (node->GetNodeType() == BpNodeType::BP_GRAPH_INPUT) {
		_input_node = node;
	}
	if (node->GetNodeType() == BpNodeType::BP_GRAPH_OUTPUT) {
		_output_node = node;
	}
	node->SetParentGraph(std::dynamic_pointer_cast<BpGraph>(shared_from_this()));
	_nodes.push_back(node);
}

int BpGraph::AddLink(BpPin& start_pin, BpPin& end_pin) {
	_links.emplace_back(GetNextID(), start_pin.ID, end_pin.ID);
	start_pin.SetLinked();
	end_pin.SetLinked();
	return _links.back().ID;
}

void BpGraph::DelLink(int id) {
	for (auto it = _links.begin(); it != _links.end(); ++it) {
		if ((*it).ID == id) {
			BpPin* sp = SearchPin((*it).StartPinID);
			BpPin* ep = SearchPin((*it).EndPinID);
			sp->SetLinked(false);
			ep->SetLinked(false);
			_links.erase(it);
			break;
		}
	}
}
void BpGraph::DelNode(std::shared_ptr<BpNode> node) {
	for (auto it = _nodes.begin(); it != _nodes.end(); ++it) {
		if (*it == node) {
			if (node->GetNodeType() == BpNodeType::BP_NODE_VAR) {
				LOG(INFO) << "cur var " << node->GetName() << "use count: " << _vars[node->GetName()]._var.use_count();
				if (_vars[node->GetName()]._var.use_count() == 2) {
					LOG(INFO) << "Erase var " << node->GetName();
					_vars.erase(node->GetName());
				}
			}
			_nodes.erase(it);
			break;
		}
	}
}

void BpGraph::DelNode(int id) {
	for (auto it = _nodes.begin(); it != _nodes.end(); ++it) {
		if ((*it)->GetID() == id) {
			DelNode(*it);
			return;
		}
	}
}

std::vector<BpLink> BpGraph::SearchLinks(int id) {
	std::vector<BpLink> ret;
	for (int i = 0; i < _links.size(); ++i) {
		if (_links[i].StartPinID == id || _links[i].EndPinID == id) {
			ret.push_back(_links[i]);
		}
	}
	return ret;
}

BpLink BpGraph::GetLink(int id) {
	for (int i = 0; i < _links.size(); ++i) {
		if (_links[i].ID == id) {
			return _links[i];
		}
	}
	return BpLink(0, 0, 0);
}

BpPin* BpGraph::SearchPin(int id) {
	for (auto p : _event_nodes) {
		auto com = p.second;
		std::vector<BpPin>& inputs = com->GetPins(BpPinKind::BP_INPUT);
		std::vector<BpPin>& outputs = com->GetPins(BpPinKind::BP_OUTPUT);
		for (BpPin& pin : inputs) {
			if (pin.ID == id)
				return &pin;
		}
		for (BpPin& pin : outputs) {
			if (pin.ID == id)
				return &pin;
		}
	}
	for (auto com : _nodes) {
		std::vector<BpPin>& inputs = com->GetPins(BpPinKind::BP_INPUT);
		std::vector<BpPin>& outputs = com->GetPins(BpPinKind::BP_OUTPUT);
		for (BpPin& pin : inputs) {
			if (pin.ID == id)
				return &pin;
		}
		for (BpPin& pin : outputs) {
			if (pin.ID == id)
				return &pin;
		}
	}
	return nullptr;
}

std::shared_ptr<BpNode> BpGraph::GetNode(int id) {
	for (auto com : _nodes) {
		if (com->GetID() == id) return com;
	}
	return nullptr;
}

bool BpGraph::AddEventNode(std::shared_ptr<BpNode> node) {
	if (_node_type == BpNodeType::BP_GRAPH) {
		LOG(ERROR) << _name << " not exec graph";
		return false;
	}
	if (node->GetNodeType() != BpNodeType::BP_NODE_EV) {
		LOG(ERROR) << node->GetName() << " not ev node";
		return false;
	}
	auto ev_node = std::dynamic_pointer_cast<BpNodeEv>(node);
	if (_event_nodes.find(node->GetName()) == _event_nodes.end()) {
		ev_node->SetParentGraph(std::dynamic_pointer_cast<BpGraph>(shared_from_this()));
		_event_nodes[ev_node->GetName()] = ev_node;
		return true;
	}
	LOG(ERROR) << "add event " << ev_node->GetName() << "failed";
	return false;
}

void BpGraph::DelEventNode(std::string name) {
	if (_event_nodes.find(name) != _event_nodes.end()) {
		auto com = _event_nodes[name];
		_event_nodes.erase(name);
	}
}

void BpGraph::ClearFlag() {
	for (auto& ev_com : _event_nodes) {
		ev_com.second->ClearFlag();
	}
	for (auto& com : _nodes) {
		com->ClearFlag();
	}
}

void BpGraph::RunNextEventBeign() {
	_event_nodes_run.clear();
	for (auto& p : _event_nodes) {
		LOG(INFO) << "Insert event node " << p.first;
		p.second->ResetLoop();
		_event_nodes_run.insert(p.second);
	}
}

bool BpGraph::RunNextEvent() {
	ClearFlag();
	for (const auto& it : _event_nodes_run) {
		auto cnt = it->LoopOnce();
		if (cnt >= 0) {
			it->Run();
		}
		if (cnt == 0) {
			LOG(INFO) << "erase event node: " << it->GetName();
			_event_nodes_run.erase(it);
		}
		return cnt >= 0;
	}
	return false;
}

void BpGraph::Run() {
	if (_node_type == BpNodeType::BP_GRAPH) {
		ClearFlag();
		BpNode::Run();
	} else {
		RunNextEventBeign();
		while(RunNextEvent());
	}
}

} // namespace bp