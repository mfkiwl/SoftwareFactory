#pragma once
#include "SFEPanel.hpp"

namespace sfe {

class SFEPanelLog : public SFEPanel {
public:
    bool Init() override;

    void Update() override;

    void Exit() override;

    void AddLogCB(const std::string& msg);
    
private:
    void Clear();
    void AddLog(const char* fmt, ...) IM_FMTARGS(2);

    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
    bool                AutoScroll = true;     // Keep scrolling if already at the bottom
};

} // namespace sfe