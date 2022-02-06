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
        v["command"] = _inputs[1].Get<::bp::String>()->var();
        v["x"] = _inputs[2].Get<::bp::Float>()->var();;
        v["y"] = _inputs[3].Get<::bp::Float>()->var();;
        p->SendMessage({p->PanelName(), "plot", "", v});
    }

private:
    std::weak_ptr<SFEPanel> _panel;
};

} // namespace sfe