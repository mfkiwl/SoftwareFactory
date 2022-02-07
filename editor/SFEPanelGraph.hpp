#pragma once
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanelGraph : public SFEPanel {
public:
    virtual bool Init() override;

    virtual void Update() override;

    virtual void Exit() override;

    virtual void OnMessage(const SFEMessage& msg) override;

private:
    void ShowVarSetting();
    void ShowVariable();
    void ShowNodes();
    std::string _set_graph = "";
    bool _runing = false;
    int _run_state = 1;
    int _drag_state = 0;
    std::string _drag_var_name = "";
    bool _show_var_setting = false;
};

} // namespace sfe