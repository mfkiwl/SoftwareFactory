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
    std::string _set_graph = "";
    bool _runing = false;
};

} // namespace sfe