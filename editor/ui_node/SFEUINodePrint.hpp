#pragma once
#include "BpNode.hpp"
#include "SFEPanel.hpp"

namespace sfe {

class SFEUINodePrint : public bp::BpNode {
public:
    SFEUINodePrint(const std::string& name, std::shared_ptr<bp::BpGraph> parent, std::shared_ptr<SFEPanel> panel)
        : BpNode(name, parent)
        , _panel(panel)
    {
        _node_type = bp::BpNodeType::BP_NODE_USER;
    }

	virtual void Logic() {
        if (_panel.expired()) {
            LOG(ERROR) << "Print node has no panel";
            return;
        }
        for (int i = 0; i < _inputs.size(); ++i) {
            if (_inputs[i].GetPinType() == ::bp::BpPinType::BP_VALUE) {
                if (_inputs[i].GetValue() != nullptr) {
                    Json::Value v;
                    _panel.lock()->UILog(_inputs[i].GetValue()->DebugString());
                } else {
                    LOG(ERROR) << "value is nullptr";
                }
            }
        }
    }

private:
    std::weak_ptr<SFEPanel> _panel;
};

} // namespace sfe