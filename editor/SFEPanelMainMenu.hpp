#pragma once
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanelMainMenu : public SFEPanel {
public:
    virtual bool Init() override;

    virtual void Update() override;

    virtual void Exit() override;

private:
    void CreateNewGraph();
    void ImportGraph();
    void SaveGraph();

    bool _runing = false;
};

} // namespace sfe