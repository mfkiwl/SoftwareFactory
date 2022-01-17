#include "SFEPanelMainMenu.hpp"

namespace sfe {

bool SFEPanelMainMenu::Init() {
    SetPanelName("mainmenu");
    return true;
}

void SFEPanelMainMenu::Update() {
    static bool create_new = false;
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Create New", "")) {
                LOG(INFO) << "Create New";
                create_new = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Import...", "")) {}
            if (ImGui::MenuItem("Export...", "")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Save...", "CTRL+S")) {
                LOG(INFO) << "Save...";
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
            if (ImGui::MenuItem("Start", "F5", false, !_runing)) {
                _runing = true;
                Json::Value v;
                v["command"] = "run_cur_graph";
                v["run"] = _runing;
                SendMessage({PanelName(), "all", "", v});
            }
            if (ImGui::MenuItem("Pause", "CTRL+F5")) {}
            if (ImGui::MenuItem("Stop", "SHIFT+F5", false, _runing)) {
                _runing = false;
                Json::Value v;
                v["command"] = "run_cur_graph";
                v["run"] = _runing;
                SendMessage({PanelName(), "all", "", v});
            }
            if (ImGui::MenuItem("Restart", "CTRL+SHIFT+F5")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("ShowDemo")) {
                SendMessage({"", "editor", "show_demo"});
            }
            if (ImGui::MenuItem("ErrorCommand")) {
                SendMessage({"", "xxx", "show_demo"});
            }
            if (ImGui::MenuItem("Spawn node")) {
                SendMessage({"", "bp editor", "spawn_node"});
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (create_new) {
        create_new = false;
        ImGui::OpenPopup("create graph...");
    }
    CreateNewGraph();
}

void SFEPanelMainMenu::Exit() {

}

void SFEPanelMainMenu::CreateNewGraph() {
    if (ImGui::BeginPopupModal("create graph...", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static std::string graph_type = "exec graph";
        static int style_idx = 0;
        if (ImGui::Combo("graph type", &style_idx, "exec graph\0mod graph\0")) {
            switch (style_idx) {
            case 0: graph_type = "exec graph"; break;
            case 1: graph_type = "mod graph"; break;
            }
        }
        static char buf[64] = "";
        ImGui::InputText("graph name", buf, 64, ImGuiInputTextFlags_CharsNoBlank);

        if (ImGui::Button("OK", ImVec2(120, 0))) { 
            Json::Value v;
            v["command"] = "create_new";
            v["graph_type"] = graph_type;
            v["graph_name"] = buf;
            SendMessage({PanelName(), "editor", "", v});
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

} // namespace sfe