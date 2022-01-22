#include <google/protobuf/text_format.h>
#include "bpbase.pb.h"
#include "SFEPanelPlot.hpp"
#include "imgui_internal.h"

namespace sfe {

bool SFEPanelPlot::Init() {
    SetPanelName("plot");
    return true;
}

void SFEPanelPlot::Update() {
    static ImU32 colors[1] = { ImColor(0, 0, 255) };
    static uint32_t selection_start = 0, selection_length = 0;

    ImGui::Begin("plot", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGuiWindow* cur_win = ImGui::GetCurrentWindow();
    
    // Draw first plot with multiple sources
    ImGui::PlotConfig conf;
    conf.values.xs = &_x_data[0];
    conf.values.ys = &_y_data[0];
    conf.values.count = _x_data.size();
    
    conf.values.ys_list = nullptr; // use ys_list to draw several lines simultaneously
    conf.values.ys_count = 0;
    conf.values.colors = colors;
    conf.scale.min = -1;
    conf.scale.max = 1;
    conf.tooltip.show = true;
    conf.grid_x.show = true;
    conf.grid_x.size = 128;
    conf.grid_x.subticks = 4;
    conf.grid_y.show = true;
    conf.grid_y.size = 0.5f;
    conf.grid_y.subticks = 5;
    conf.selection.show = true;
    conf.selection.start = &selection_start;
    conf.selection.length = &selection_length;
    float height = selection_length > 1 ? (cur_win->Size.y - 30) / 2 : (cur_win->Size.y - 30);
    conf.frame_size = ImVec2(cur_win->Size.x, height);
    ImGui::Plot("plot1", conf);

    if (selection_length > 1) {
        // Draw second plot with the selection
        // reset previous values
        conf.values.ys_list = nullptr;
        conf.selection.show = false;
        // set new ones
        conf.values.ys = &_y_data[0];
        conf.values.offset = selection_start;
        conf.values.count = selection_length;
        conf.line_thickness = 2.f;
        ImGui::Plot("plot2", conf);
    }

    ImGui::End();
}

void SFEPanelPlot::Exit() {

}

void SFEPanelPlot::OnMessage(const SFEMessage& msg) {
    /*
    绘制命令
        string cmd: {
            cmd: ["plot" | "clear"]
            figure: 1
            color: 
        }
        std::vector<BpFloatPair> _data:
    */
    if (msg.msg.empty()) {
        auto cmd = msg.json_msg["command"].asString();
        if (cmd == "plot") {
            _x_data.push_back(msg.json_msg["x"].asFloat());
            _y_data.push_back(msg.json_msg["y"].asFloat());
        }
    }
}

} // namespace sfe