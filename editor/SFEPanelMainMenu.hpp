#pragma once
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanelMainMenu : public SFEPanel {
public:
    virtual bool Init() override;

    virtual void Update() override;

    virtual void Exit() override;

    virtual void OnMessage(const SFEMessage& msg) override;

private:
    void CreateNewGraph();
    void OpenGraph();
    void SaveGraph();

    bool _runing = false;
};

} // namespace sfe