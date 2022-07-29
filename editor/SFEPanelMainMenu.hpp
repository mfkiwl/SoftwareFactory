#pragma once
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanelMainMenu : public SFEPanel {
public:
    bool Init() override;

    void Update() override;

    void Exit() override;

    void OnMessage(const SFEMessage& msg) override;

private:
    void CreateNewGraph();
    void OpenGraph();
    void SaveGraph();

    bool _runing = false;
    int _debug_mode = 1;
};

} // namespace sfe