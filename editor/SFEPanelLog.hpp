#pragma once
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanelLog : public SFEPanel {
public:
    virtual bool Init() override;

    virtual void Update() override;

    virtual void Exit() override;

    virtual void OnMessage(const SFEMessage& msg) override;

private:
    void Clear();
    void AddLog(const char* fmt, ...) IM_FMTARGS(2);

    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
    bool                AutoScroll = true;     // Keep scrolling if already at the bottom
};

} // namespace sfe