#pragma once
#include "SFEPanel.hpp"
#include "BpContents.hpp"

namespace sfe {

class SFEPanelLib : public SFEPanel {
public:
    virtual bool Init() override;

    virtual void Update() override;

    virtual void Exit() override;

private:
    void ShowLib(std::shared_ptr<BpContents> c);

    int _drag_state = 0;
    std::weak_ptr<BpContents> _drag_item;
};

} // namespace sfe