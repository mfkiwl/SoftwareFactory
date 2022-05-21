#pragma once
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanelGraph : public SFEPanel {
public:
    bool Init() override;

    void Update() override;

    void Exit() override;

    void OnMessage(const SFEMessage& msg) override;

private:
    void ShowVarSetting();
    void ShowVariable();
    void ShowNodes();
    std::string _set_graph = "";
    bool _runing = false;
    int _drag_state = 0;
    std::string _drag_var_name = "";
    bool _show_var_setting = false;
};

} // namespace sfe