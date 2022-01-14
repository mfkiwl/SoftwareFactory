#include "SFEditor.hpp"
#include "SFEPanelMainMenu.hpp"
#include "Bp.hpp"

namespace sfe {

bool SFEditor::Init() {
    _panels.emplace_back(std::make_shared<SFEPanelMainMenu>());

    for (auto it = _panels.begin(); it != _panels.end(); ++it) {
        (*it)->Init();
    }
    return true;
}

void SFEditor::ProcEvent(const SDL_Event event) {
    for (auto it = _panels.begin(); it != _panels.end(); ++it) {
        (*it)->ProcEvent(event);
    }
}

void SFEditor::Update() {
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (_show_demo) {
        ImGui::ShowDemoWindow(&_show_demo);
    }

    for (auto it = _panels.begin(); it != _panels.end(); ++it) {
        (*it)->Update();
    }
}

void SFEditor::DispatchMessage() {
    for (auto it = _panels.begin(); it != _panels.end(); ++it) {
        auto msgs = (*it)->GetDispatchMessage();
        for (int j = 0; j < msgs.size(); ++j) {
            if (msgs[j].dst == "editor") {
                ProcEditorMessage(msgs[j]);
                continue;
            }
            auto panel = GetPanel(msgs[j].dst);
            if (panel == nullptr) {
                LOG(ERROR) << "Can't find panel, msg: " << msgs[j].Print();
                continue;
            }
            panel->RecvMessage(msgs[j]);
        }
        (*it)->ClearDispathMessage();
    }
}

void SFEditor::ProcMessage() {
    for (auto it = _panels.begin(); it != _panels.end(); ++it) {
        (*it)->ProcMessage();
    }
}

void SFEditor::ProcEditorMessage(const SFEMessage& msg) {
    if (msg.msg == "showdemo") {
        _show_demo = !_show_demo;
    }
}

void SFEditor::Exit() {
    for (auto it = _panels.begin(); it != _panels.end(); ++it) {
        (*it)->Exit();
    }
}

const std::shared_ptr<SFEPanel> SFEditor::GetPanel(const std::string& name) {
    for (auto it = _panels.begin(); it != _panels.end(); ++it) {
        if ((*it)->PanelName() == name) {
            return (*it);
        }
    }
    return nullptr;
}

} // namespace sfe