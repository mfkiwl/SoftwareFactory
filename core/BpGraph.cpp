﻿#include <fstream>

#include "Bp.hpp"
#include "BpGraph.hpp"
#include "BpPin.hpp"
#include "BpLink.hpp"

namespace bp {

BpGraph::BpGraph(std::shared_ptr<BpGraph> parent) 
	: BpNode("", parent) {}

BpGraph::BpGraph(std::string name, BpNodeType t, std::shared_ptr<BpGraph> parent)
	: BpNode(name, parent) {
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
			BpPin* sp = GetPin((*it).StartPinID);
			BpPin* ep = GetPin((*it).EndPinID);
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

const BpLink* BpGraph::GetLinkByPinID(int pin_id) {
	for (int i = 0; i < _links.size(); ++i) {
		if (_links[i].StartPinID == pin_id || _links[i].EndPinID == pin_id) {
			return &_links[i];
		}
	}
	return nullptr;
}

BpLink* BpGraph::GetLink(int id) {
	for (int i = 0; i < _links.size(); ++i) {
		if (_links[i].ID == id) {
			return &_links[i];
		}
	}
	return nullptr;
}

BpPin* BpGraph::GetPin(int id) {
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

std::shared_ptr<BpNode> BpGraph::GetNextNodeByOutPinID(int pin_id) {
	auto link_id = GetLinkByPinID(pin_id);
	auto pin = GetPin(link_id->EndPinID);
	return pin->GetObj();
}

std::shared_ptr<BpNode> BpGraph::GetPreNodeByInPinID(int pin_id) {
	auto link_id = GetLinkByPinID(pin_id);
	auto pin = GetPin(link_id->StartPinID);
	return pin->GetObj();
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

void BpGraph::ClearChildrenFlagByOutPinID(int pin_id) {
	auto link_id = GetLinkByPinID(pin_id);
	ClearChildrenFlagByInPinID(link_id->EndPinID);
}

void BpGraph::ClearChildrenFlagByInPinID(int pin_id) {
	auto pin = GetPin(pin_id);
	auto node = pin->GetObj();
	if (node == nullptr) {
		return;
	}
	std::unordered_set<std::shared_ptr<BpNode>> visited;
	ClearChildrenFlagHelper(pin_id, visited, node);
}

void BpGraph::ClearChildrenFlagHelper(int pin_id, std::unordered_set<std::shared_ptr<BpNode>>& visited, std::shared_ptr<BpNode> node) {
	if (visited.find(node) != visited.end()) {
		return;
	}
	auto& in_pins = node->GetPins(BpPinKind::BP_INPUT);
	for (int i = 0; i < in_pins.size(); ++i) {
		if (pin_id == in_pins[i].ID) {
			continue;
		}
		if (in_pins[i].IsLinked() 
			&& in_pins[i].GetPinType() != BpPinType::BP_FLOW) {
			auto pre_node = GetPreNodeByInPinID(in_pins[i].ID);
			ClearChildrenFlagHelper(pin_id, visited, pre_node);
		}
	}
	
	visited.insert(node);
	node->ClearFlag();

	auto& out_pins = node->GetPins(BpPinKind::BP_OUTPUT);
	for (int i = 0; i < out_pins.size(); ++i) {
		if (out_pins[i].IsLinked() && out_pins[i].GetPinType() == BpPinType::BP_FLOW) {
			auto next_node = GetNextNodeByOutPinID(out_pins[i].ID);
			ClearChildrenFlagHelper(pin_id, visited, next_node);
		}
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

BpNodeRunState BpGraph::RunNextEvent() {
	ClearFlag();
	BpNodeRunState run_state = BpNodeRunState::BP_RUN_OK;
	for (const auto& it : _event_nodes_run) {
		auto cnt = it->LoopOnce();
		if (cnt >= 0) {
			run_state = it->Run();
		}
		if (cnt == 0) {
			LOG(INFO) << "erase event node: " << it->GetName();
			_event_nodes_run.erase(it);
		}
		return run_state;
	}
	return BpNodeRunState::BP_RUN_FINISH;
}

BpNodeRunState BpGraph::Run() {
	if (_node_type == BpNodeType::BP_GRAPH) {
		ClearFlag();
		return BpNode::Run();
	} else {
		RunNextEventBeign();
		while(BpNodeRunState::BP_RUN_FINISH != RunNextEvent());
	}
	return BpNodeRunState::BP_RUN_OK;
}

void BpGraph::StartDebug() { 
	_debug_mode = true;
	RunNextEventBeign(); 
}

BpNodeRunState BpGraph::ContinueDebug() {
	_debug_cur_links_flow.clear();
	/// 继续执行下一个没执行完成的节点
	if (_breakpoint_node.expired()) {
		return RunNextEvent();
	}
	auto bp_node = _breakpoint_node.lock();
	auto run_state = bp_node->Run();
	if (run_state == BpNodeRunState::BP_RUN_OK
		|| run_state == BpNodeRunState::BP_RUN_LOOP_INTERNAL) {
		// 执行loop stack的节点
		if (!_breakpoint_loop_stack.empty()) {
			auto loop_node = _breakpoint_loop_stack.top();
			_breakpoint_node = loop_node;
			_breakpoint_loop_stack.pop();
			return BpNodeRunState::BP_RUN_BREAKPOINT;
		}
		_breakpoint_node.reset();
		if (_event_nodes_run.empty()) {
			run_state = BpNodeRunState::BP_RUN_FINISH;
		}
	}
	return run_state;
}

void BpGraph::StopDebug() {
	_debug_mode = false;
}

void BpGraph::SetAllBreakpoints(bool b) {
	for (auto& com : _nodes) {
		com->SetBreakpoint(b);
	}
}

} // namespace bp