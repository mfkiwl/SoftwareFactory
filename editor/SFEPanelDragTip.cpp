#include "SFEPanelDragTip.hpp"

namespace sfe {

bool SFEPanelDragTip::Init() {
    SetPanelName("drag tip");
    return true;
}

void SFEPanelDragTip::Update() {
    if (!_visible) return;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize;
	ImVec2 xy = ImGui::GetMousePos();
	xy.x -= _desc.length() * 8 / 2;
	xy.y += 10;
	ImGui::SetNextWindowPos(xy);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(255, 255, 255, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("##dragdropwindow", 0, window_flags);
	ImGui::PopStyleVar(3);
	ImGui::PopStyleColor();
	ImGui::TextUnformatted(_desc.c_str());
	ImGui::End();
}

void SFEPanelDragTip::Exit() {
}

void SFEPanelDragTip::OnMessage(const SFEMessage& msg) {
    if (msg.msg.empty()) {
        auto cmd = msg.json_msg["command"].asString();
        if (cmd == "show") {
            _visible = true;
            _desc = msg.json_msg["desc"].asString();
        }
        if (cmd == "close") {
            _visible = false;
        }
    }
}

} // namespace sfe