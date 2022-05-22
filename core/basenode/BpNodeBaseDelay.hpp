#pragma once
#include "BpNode.hpp"
#include "Bp.hpp"
#include "bpcommon.hpp"
#include "bpbase.pb.h"

namespace bp {

class BpNodeBaseDelay : public BpNode
{
public:
	BpNodeBaseDelay(std::shared_ptr<BpGraph> parent, float duration)
        : BpNode("Delay", parent)
        , _duration(duration)
    {
        _node_type = bp::BpNodeType::BP_NODE_BASE;
    }

    void Init() override {
        AddPin("", BpPinKind::BP_INPUT, BpPinType::BP_FLOW, BpVariable());
        auto var = Bp::Instance().CreateVariable("bpbase.Float");
        AddPin("Duration", BpPinKind::BP_INPUT, BpPinType::BP_VALUE, var);
        
        auto& pin = AddPin("Completed", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable());
        pin.SetExecutable(true);
    }

	BpNodeRunState Logic() override {
        if (_outputs[0].IsExecutable()) {
            _time = 0;
            _last_timestamp = BpCommon::GetTimestamp();
            _outputs[0].SetExecutable(false);
        }
        else {
            double cur_timestamp = BpCommon::GetTimestamp();
            _time += (cur_timestamp - _last_timestamp);
            _last_timestamp = cur_timestamp;
            float duration = _inputs[1].GetBpValue().Get<bp::Float>()->var();
            if (_time >= duration) {
                _outputs[0].SetExecutable(true);
            }
        }
        return BpNodeRunState::BP_RUN_LOGIC_OK;
    };
    
private:
    float _duration = .0f;
    double _last_timestamp;
    double _time;
};

} // namespace bp