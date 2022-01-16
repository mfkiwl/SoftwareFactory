#pragma once
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanelDragTip : public SFEPanel {
public:
    virtual bool Init() override;

    virtual void Update() override;

    virtual void Exit() override;

    virtual void OnMessage(const SFEMessage& msg) override;

private:
    bool _visible = false;
    std::string _desc = "";
};

} // namespace sfe