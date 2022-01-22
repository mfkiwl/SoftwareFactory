#pragma once
#include "BpNode.hpp"
#include "SFEPanel.hpp"
#include "bpbase.pb.h"

namespace sfe {

class SFEUINodePlot : public bp::BpNode {
public:
    SFEUINodePlot(const std::string& name, std::shared_ptr<bp::BpGraph> parent, std::shared_ptr<SFEPanel> panel)
        : BpNode(name, parent)
        , _panel(panel)
    {
        _node_type = bp::BpNodeType::BP_NODE_USER;
    }

	virtual void Logic() {
        if (_panel.expired()) {
            LOG(ERROR) << "Plot node has no panel";
            return;
        }
        auto p = _panel.lock();
        Json::Value v;
        v["command"] = std::dynamic_pointer_cast<::bp_pb::BpString>(_inputs[1].GetValue())->var();
        v["x"] = std::dynamic_pointer_cast<::bp_pb::BpFloat>(_inputs[2].GetValue())->var();
        v["y"] = std::dynamic_pointer_cast<::bp_pb::BpFloat>(_inputs[3].GetValue())->var();
        p->SendMessage({p->PanelName(), "plot", "", v});
    }

private:
    std::weak_ptr<SFEPanel> _panel;
};

} // namespace sfe