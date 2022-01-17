#pragma once
#include "../BpNode.hpp"

namespace bp {

class BpBaseNodePrint : public BpNode
{
public:
	BpBaseNodePrint(std::shared_ptr<BpGraph> parent)
        : BpNode("Print", parent, bp::BpObjType::BP_NODE_BASE) {}

	virtual void Logic() {
        for (int i = 0; i < _inputs.size(); ++i) {
            if (_inputs[i].GetPinType() == BpPinType::BP_VALUE) {
                if (_inputs[i].GetValue() != nullptr) {
                    LOG(INFO) << _inputs[i].GetValue()->DebugString();
                } else {
                    LOG(ERROR) << "value is nullptr";
                }
            }
        }
    }
};

} // namespace bp