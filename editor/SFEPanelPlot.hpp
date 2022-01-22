#pragma once
#include "SFEPanel.hpp"
#include "imgui_plot.h"

namespace sfe {

class SFEPanelPlot : public SFEPanel {
public:
    virtual bool Init() override;

    virtual void Update() override;

    virtual void Exit() override;

    virtual void OnMessage(const SFEMessage& msg) override;

private:
    std::vector<float> _y_data;
    std::vector<float> _x_data;
    int _max_display_sz = 2048;
};

} // namespace sfe