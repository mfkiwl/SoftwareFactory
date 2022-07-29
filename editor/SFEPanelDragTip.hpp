#pragma once
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanelDragTip : public SFEPanel {
public:
    bool Init() override;

    void Update() override;

    void Exit() override;

    void OnMessage(const SFEMessage& msg) override;

private:
    bool _visible = false;
    std::string _desc = "";
};

} // namespace sfe