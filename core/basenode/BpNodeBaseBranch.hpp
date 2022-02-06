#pragma once
#include "BpNode.hpp"
#include "Bp.hpp"
#include "bpcommon.hpp"
#include "bpbase.pb.h"

namespace bp {

class BpNodeBaseBranch : public BpNode
{
public:
	BpNodeBaseBranch(std::shared_ptr<BpGraph> parent)
        : BpNode("Branch", parent)
    {
        _node_type = bp::BpNodeType::BP_NODE_BASE;
    }

    void Init() override {
        AddPin("", BpPinKind::BP_INPUT, BpPinType::BP_FLOW, BpVariable());
        auto var = Bp::Instance().CreateVariable("bpbase.Bool", "bpbase.Bool");
        AddPin("Condition", BpPinKind::BP_INPUT, BpPinType::BP_VALUE, var);

        AddPin("True", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable()); // true
        AddPin("False", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable()); // false
    }

	void Logic() override {
        bool b = _inputs[1].GetBpValue().Get<bp::Bool>()->var();
        if (b) {
            _outputs[0].SetExecutable(true);
            _outputs[1].SetExecutable(false);
        }
        else {
            _outputs[0].SetExecutable(false);
            _outputs[1].SetExecutable(true);
        }
    };
};

} // namespace bp