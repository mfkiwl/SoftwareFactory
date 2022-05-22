#pragma once
#include "BpNode.hpp"
#include "BpGraph.hpp"
#include "Bp.hpp"
#include "bpcommon.hpp"
#include "bpbase.pb.h"

namespace bp {

class BpNodeBaseWhile : public BpNode
{
public:
	BpNodeBaseWhile(std::shared_ptr<BpGraph> parent)
        : BpNode("While", parent)
    {
        _node_type = bp::BpNodeType::BP_NODE_BASE;
    }

    void Init() override {
        AddPin("", BpPinKind::BP_INPUT, BpPinType::BP_FLOW, BpVariable());
        auto var = Bp::Instance().CreateVariable("bpbase.Bool");
        AddPin("Condition", BpPinKind::BP_INPUT, BpPinType::BP_VALUE, var);

        AddPin("Loop Body", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable());
        AddPin("Completed", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable());
    }

	BpNodeRunState Logic() override {
        auto parent = GetParentGraph();
        auto& in_pins = GetPins(BpPinKind::BP_INPUT);
        auto& out_pins = GetPins(BpPinKind::BP_OUTPUT);
        out_pins[0].SetExecutable(true);
        out_pins[1].SetExecutable(false);
        do {
            parent->ClearChildrenFlagByInPinID(in_pins[0].ID);
            BuildInput(parent);
            if (!in_pins[1].GetBpValue().Get<bp::Bool>()->var()) {
                break;
            }
            if (parent->IsDebugMode()) {
                auto flow_link = parent->GetLinkByPinID(out_pins[0].ID);
                parent->AddCurDebugLinkFlow(flow_link->ID);
            }
            auto next_node = parent->GetNextNodeByOutPinID(out_pins[0].ID);
            auto state = next_node->Run();
            if (parent->IsDebugMode()) {
                parent->PushLoopNode(shared_from_this());
                return state == BpNodeRunState::BP_RUN_OK ? BpNodeRunState::BP_RUN_LOOP_INTERNAL : state;
            }
        } while(1);
        out_pins[0].SetExecutable(false);
        out_pins[1].SetExecutable(true);
        return BpNodeRunState::BP_RUN_LOGIC_OK;
    };
};

} // namespace bp