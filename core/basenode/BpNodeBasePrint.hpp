#pragma once
#include "BpNode.hpp"

namespace bp {

class BpNodeBasePrint : public bp::BpNode {
public:
    BpNodeBasePrint(std::shared_ptr<bp::BpGraph> parent)
        : BpNode("Print", parent)
    {
        _node_type = bp::BpNodeType::BP_NODE_BASE;
    }

    void Init() override {
        AddPin("", ::bp::BpPinKind::BP_INPUT, ::bp::BpPinType::BP_FLOW, ::bp::BpVariable()); 
        AddPin("", ::bp::BpPinKind::BP_OUTPUT, ::bp::BpPinType::BP_FLOW, ::bp::BpVariable()); 
        AddPin("any", ::bp::BpPinKind::BP_INPUT, ::bp::BpPinType::BP_VALUE, ::bp::BpVariable("any", "any", nullptr)).SetAssignByRef(true); 
    }

	virtual void Logic() {
        for (int i = 0; i < _inputs.size(); ++i) {
            if (_inputs[i].GetPinType() == ::bp::BpPinType::BP_VALUE) {
                if (_inputs[i].Get<pb_msg_t>() != nullptr) {
                    LOG(INFO) << _inputs[i].Get<pb_msg_t>()->DebugString();
                } else {
                    LOG(ERROR) << "value is nullptr";
                }
                _inputs[i].SetValueByRef(nullptr);
            }
        }
    }
};

} // namespace bp