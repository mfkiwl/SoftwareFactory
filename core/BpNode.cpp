#include <glog/logging.h>

#include "BpNode.hpp"
#include "BpLink.hpp"
#include "BpGraph.hpp"

namespace bp {

BpNode::BpNode(const std::string& name, std::shared_ptr<BpGraph> parent)
	: std::enable_shared_from_this<BpNode>()
    , _parent_graph(parent)
	, _node_type(BpNodeType::BP_NODE_FUNC)
	, _node_style(BpNodeStyle::BP_BLUPRINT)
	, _name(name)
	, _tmp_next_id(0)
	, _has_breakpoint(false)
{
	if (parent) {
		_id = parent->GetNextID();
	} else {
        _id = 0;
    }
	LOG(INFO) << "Create node \"" << _name << "\": " << _id;
}

BpNode::~BpNode()
{
	_inputs.clear();
	_outputs.clear();
	LOG(INFO) << "Delete node \"" << _name << "\": " << _id;
}

void BpNode::SetParentGraph(std::shared_ptr<BpGraph> graph) {
	LOG(INFO) << _name << "(node) --> " << graph->_name << "(graph)";
	_parent_graph = graph;
	_id = graph->GetNextID();
	for (auto& out : _outputs) {
		out.ID = graph->GetNextID();
	}
	for (auto& in : _inputs) {
		in.ID = graph->GetNextID();
	}
	_tmp_next_id = 0;
}

bool BpNode::NeedBuildInputPin(const BpPin& in) {
	return (in.IsLinked() 
			&& !in.IsVaild() 
			&& in.GetPinType() != BpPinType::BP_FLOW);
}

void BpNode::BuildInput(std::shared_ptr<BpGraph>& graph) {
	/*
		如果是执行Pin，则跳过
		如果是没有连线，则跳过
		如果有连线并且是参数Pin，并且参数有效，则跳过
		如果有连线并且是参数Pin，并且参数无效，则搜索连线节点，并执行该节点
	*/
	for (BpPin& in : _inputs) {
		if (NeedBuildInputPin(in)) {
			auto link = graph->GetLinkByPinID(in.ID);
			if (link->EndPinID == in.ID) {
				graph->GetPin(link->StartPinID)->GetObj()->Run();
			}
		}
	}
}

void BpNode::BuildOutput(std::shared_ptr<BpGraph>& graph) {
	/* 
		循环1：
			如果是参数Pin并且有连线，则搜索连线节点的pin，设置该节点值
			其他则跳过
		循环2：
			如果是执行Pin并且有连线，则搜索连线节点的com，执行该com
	*/
	for (auto& out : _outputs) {
		if (out.IsLinked() && out.GetPinType() != BpPinType::BP_FLOW) {
			auto link = graph->GetLinkByPinID(out.ID);
			if (link->StartPinID == out.ID) {
				bp:BpPin* p = graph->GetPin(link->EndPinID);
				if (p->AssignByRef()) {
					p->SetValueByRef(out.Get<pb_msg_t>(), true);
				} else {
					p->SetValue(out.Get<pb_msg_t>(), true);
				}
			}
		}
	}
}

bool BpNode::IsRunableOutputPin(const BpPin& out) {
	return (out.IsLinked() && out.GetPinType() == BpPinType::BP_FLOW && out.IsExecutable());
}

BpNodeRunState BpNode::Run() {
	if (_parent_graph.expired()) {
		LOG(ERROR) << _name << " has no parent graph";
		return BpNodeRunState::BP_RUN_NO_PARENT;
	}
    auto graph = _parent_graph.lock();
	bool debug_mode = graph->IsDebugMode();

	BpNodeRunState run_state = BpNodeRunState::BP_RUN_OK;
	// build input
	BuildInput(graph);

	// 如果有断点，返回并设置该节点
	if (debug_mode && _has_breakpoint && graph->GetCurBreakPoint() != shared_from_this()) {
		graph->SetCurBreakpoint(shared_from_this());
		LOG(INFO) << "Breakpoint: Stop at " << _name;
		return BpNodeRunState::BP_RUN_BREAKPOINT;
	}

	// exec logic
	auto state = Logic();
	if (debug_mode 
		&& (state == BpNodeRunState::BP_RUN_BREAKPOINT 
			|| state == BpNodeRunState::BP_RUN_LOOP_INTERNAL)) {
		LOG(INFO) << "Logic Breakpoint: Stop at " << _name;
		return state;
	}

	// set output
	BuildOutput(graph);

	// exec next coms
	for (auto& out : _outputs) {
		if (IsRunableOutputPin(out)) {
			auto link = graph->GetLinkByPinID(out.ID);
			if (link->StartPinID == out.ID) {
				if (debug_mode) {
					graph->AddCurDebugLinkFlow(link->ID);
				}
				run_state = graph->GetPin(link->EndPinID)->GetObj()->Run();
				if (debug_mode && run_state == BpNodeRunState::BP_RUN_BREAKPOINT) {
					return run_state;
				}
			}
		}
	}

	return run_state;
}

BpPin& BpNode::AddPin(const std::string& name, BpPinKind k, BpPinType t, const BpVariable& v) {
	int id = 0;
	if (_parent_graph.expired()) {
		id = ++_tmp_next_id;
	} else {
		id = _parent_graph.lock()->GetNextID();
	}
	
	if (k == BpPinKind::BP_INPUT) {
		_inputs.emplace_back(shared_from_this(), k, t, id, name, v);
		return _inputs.back();
	}
	if (k == BpPinKind::BP_OUTPUT) {
		_outputs.emplace_back(shared_from_this(), k, t, id, name, v);
		return _outputs.back();
	}
	LOG(ERROR) << "Unknown pin kind " << (int)k;
    return _null_pin;
}

bool BpNode::DelPin(int id) {
	for (auto it = _inputs.begin(); it != _inputs.end(); ++it) {
		if ((*it).ID == id) {
			_inputs.erase(it);
			return true;
		}
	}
	for (auto it = _outputs.begin(); it != _outputs.end(); ++it) {
		if ((*it).ID == id) {
			_outputs.erase(it);
			return true;
		}
	}
    return false;
}

std::vector<BpPin>& BpNode::GetPins(BpPinKind k) {
	if (k == BpPinKind::BP_INPUT)
		return _inputs;
	if (k == BpPinKind::BP_OUTPUT)
		return _outputs;
	LOG(ERROR) << "Unknown pin kind " << (int)k;
    return _null_pins;
}

void BpNode::ClearFlag() {
	for (auto& in : _inputs) {
		in.SetVaild(false);
	}
	for (auto& out : _outputs) {
		out.SetVaild(false);
	}
}

void BpNode::SetBreakpoint(bool b) {
	if (_node_type == BpNodeType::BP_NODE_VAR 
		|| _node_type == BpNodeType::BP_NONE
		|| _node_type == BpNodeType::BP_GRAPH_EXEC
		|| _node_type == BpNodeType::BP_GRAPH_INPUT
		|| _node_type == BpNodeType::BP_GRAPH_OUTPUT) {
		return;
	}
	_has_breakpoint = b;
}

} // namespace bp