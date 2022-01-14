#include <iostream>
#include "SFEPanelMainMenu.hpp"

namespace sfe {

bool SFEPanelMainMenu::Init() {
    return true;
}

void SFEPanelMainMenu::Update() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Import...", "")) {}
            if (ImGui::MenuItem("Export...", "")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Save...", "CTRL+S")) {
                std::cout << "Save..." << std::endl;
            }
            if (ImGui::MenuItem("Save as...", "CTRL+SHIFT+S")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "CTRL+Q")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Run")) {
            if (ImGui::MenuItem("Start", "F5")) {}
            if (ImGui::MenuItem("Pause", "CTRL+F5")) {}
            if (ImGui::MenuItem("Stop", "SHIFT+F5")) {}
            if (ImGui::MenuItem("Restart", "CTRL+SHIFT+F5")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("ShowDemo")) {
                SendMessage({"", "editor", "showdemo"});
            }
            if (ImGui::MenuItem("ErrorCommand")) {
                SendMessage({"", "xxx", "showdemo"});
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void SFEPanelMainMenu::Exit() {

}

} // namespace sfe