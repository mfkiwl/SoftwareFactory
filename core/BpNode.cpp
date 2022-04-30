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

void BpNode::Run() {
	if (_parent_graph.expired()) {
		LOG(ERROR) << _name << " has no parent graph";
		return;
	}
    auto graph = _parent_graph.lock();

	// build input
	/*
		如果是执行Pin，则跳过
		如果是没有连线，则跳过
		如果有连线并且是参数Pin，并且参数有效，则跳过
		如果有连线并且是参数Pin，并且参数无效，则搜索连线节点，并执行该节点
	*/
	for (BpPin& in : _inputs) {
		if (in.IsLinked() && !in.IsVaild() && in.GetPinType() != BpPinType::BP_FLOW) {
			auto links = graph->SearchLinks(in.ID);
			for (auto& link : links) {
				if (link.EndPinID == in.ID) {
					graph->SearchPin(link.StartPinID)->GetObj()->Run();
				}
			}
		}
	}

	// exec logic
	Logic();

	// set output
	/* 
		循环1：
			如果是参数Pin并且有连线，则搜索连线节点的pin，设置该节点值
			其他则跳过
		循环2：
			如果是执行Pin并且有连线，则搜索连线节点的com，执行该com
	*/
	for (auto& out : _outputs) {
		if (out.IsLinked() && out.GetPinType() != BpPinType::BP_FLOW) {
			auto links = graph->SearchLinks(out.ID);
			for (auto& link : links) {
				if (link.StartPinID == out.ID) {
					bp:BpPin* p = graph->SearchPin(link.EndPinID);
					if (p->AssignByRef()) {
						p->SetValueByRef(out.Get<pb_msg_t>(), true);
					} else {
						p->SetValue(out.Get<pb_msg_t>(), true);
					}
				}
			}
		}
	}

	// exec next coms
	for (auto& out : _outputs) {
		if (out.IsLinked() && out.GetPinType() == BpPinType::BP_FLOW && out.IsExecutable()) {
			auto links = graph->SearchLinks(out.ID);
			for (auto& link : links) {
				if (link.StartPinID == out.ID) {
					graph->SearchPin(link.EndPinID)->GetObj()->Run();
				}
			}
		}
	}
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

} // namespace bp