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
    void ShowVarSetting();
    bool _show_var_setting = false;
    int _drag_state = 0;
    std::weak_ptr<BpContents> _drag_item;
    Json::Value _drag_info;
};

} // namespace sfe