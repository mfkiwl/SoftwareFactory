#include "SFEPanelGraph.hpp"
#include "Bp.hpp"
#include "BpGraph.hpp"

namespace sfe {

bool SFEPanelGraph::Init() {
    SetPanelName("graph");
    return true;
}

void SFEPanelGraph::Update() {
    ImGui::Begin(PanelName().c_str());
    const auto& edit_graphs = bp::Bp::Instance().GetEditGraphs();
    int graph_sz = edit_graphs.size();
    static int item_current = 0;
    if (graph_sz > 0 && !_runing) {
        static std::string items;
        items.clear();
        int i = 0;
        auto cur_g = nullptr;
        for(auto& g : edit_graphs) {
            if (!_set_graph.empty() && _set_graph == g.first) {
                item_current = i;
                _set_graph.clear();
            }
            items.append(g.first);
            items.append(1, '\0');
            i++;
        }
        if (ImGui::Combo("graph", &item_current, items.data())) {
            int j = 0;
            for(auto& g : edit_graphs) {
                if (item_current == j) {
                    LOG(INFO) << "graph change: " << g.first;
                    bp::Bp::Instance().SetCurEditGraph(g.second);
                    break;
                }
                j++;
            }
        }
    } else if (graph_sz > 0) {
        auto graph_name = bp::Bp::Instance().CurEditGraph()->GetName();
        ImGui::TextDisabled("%s", graph_name.c_str());
    }
    ImGui::Separator();
    ImGui::TextUnformatted("Variables");
    // 显示变量
    ShowVariable();
    ImGui::Separator();
    ImGui::TextUnformatted("Nodes");
    // 显示节点
    ShowNodes();
    // 设置变量节点
    if (_show_var_setting) {
        _show_var_setting = false;
        ImGui::OpenPopup("named variable setting");
    }
    ShowVarSetting();

    ImGui::End();

    if (_runing) {
        auto g = bp::Bp::Instance().CurEditGraph();
        if (g == nullptr) {
            return;
        }
        if (g->GetNodeType() == bp::BpNodeType::BP_GRAPH) {
            g->ClearFlag();
            g->Logic();
        } else {
            g->Run();
        }
    }
}

void SFEPanelGraph::Exit() {

}

void SFEPanelGraph::ShowNodes() {
    if (bp::Bp::Instance().CurEditGraph() == nullptr) {
        return;
    }
    auto& nodes = bp::Bp::Instance().CurEditGraph()->GetNodes();
    for (auto& node : nodes) {
        std::string title = node->GetName() + "#" + std::to_string(node->GetID());
        if (ImGui::Selectable(title.c_str())) {
            // 显示详情
            LOG(INFO) << "select node " << title;
        }
    }
    auto& ev_nodes = bp::Bp::Instance().CurEditGraph()->GetEvNodes();
    for (auto& ev_node : ev_nodes) {
        auto node = ev_node.second;
        std::string title = node->GetName() + "#" + std::to_string(node->GetID());
        if (ImGui::Selectable(title.c_str())) {
            // 显示详情
            LOG(INFO) << "select ev node " << title;
        }
    }
}

void SFEPanelGraph::ShowVariable() {
    if (bp::Bp::Instance().CurEditGraph() == nullptr) {
        return;
    }
    auto& vars = bp::Bp::Instance().CurEditGraph()->GetVariables();
    for(auto& var : vars) {
        auto title = var.second.GetName() + "(" + var.second.GetType() + ")";
        if (ImGui::Selectable(title.c_str())) {
            // 显示详情
            LOG(INFO) << "select var " << title;
        }
        // 初始状态&&在选中的item上鼠标按下，设置开始拖动
        if (_drag_state == 0 && ImGui::IsMouseDown(0) && ImGui::IsItemHovered()) {
            _drag_state = 1;
            _drag_var_name = var.second.GetName();
        }
        // 在开始拖动状态下，鼠标移动，设置拖动中状态
        ImVec2 mouse_delta = ImGui::GetMouseDragDelta(0);
        if (_drag_state == 1 && (mouse_delta.x != 0 || mouse_delta.y != 0)) {
            _drag_state = 2;
            // 给drag window发送展示消息
            Json::Value v;
            v["command"] = "show";
            v["desc"] = _drag_var_name;
            SendMessage({PanelName(), "drag tip", "", v});
        }
        // 鼠标松开，初始化状态
        if (_drag_state == 2 && ImGui::IsMouseReleased(0)) {
            // 给drag tip发送关闭展示消息
            Json::Value v;
            v["command"] = "close";
            SendMessage({PanelName(), "drag tip", "", v});
            // 给 bp editor发送创建Node消息
            // 变量节点, 需要使用界面设置get/set
            _show_var_setting = true;
            _drag_state = 0;
        }
    }
}

void SFEPanelGraph::ShowVarSetting() {
    if (ImGui::BeginPopupModal("named variable setting", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static bool is_get = true;
        static int style_idx = 0;
        if (ImGui::Combo("get/set", &style_idx, "get\0set\0")) {
            switch (style_idx) {
            case 0: is_get = true; break;
            case 1: is_get = false; break;
            }
        }
        static char buf[64] = "";
        strcpy(buf, _drag_var_name.c_str());
        ImGui::InputText("variable name", buf, 64, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_ReadOnly);

        if (ImGui::Button("OK", ImVec2(120, 0))) { 
            Json::Value v2;
            v2["command"] = "spawn_node";
            v2["node_name"] = "";
            v2["type"] = (int)bp::BpNodeType::BP_NODE_VAR;
            v2["is_get"] = is_get;
            v2["var_name"] = buf;
            ImVec2 xy = ImGui::GetMousePos();
            v2["x"] = xy.x;
            v2["y"] = xy.y;
            SendMessage({PanelName(), "editor", "", v2});
            _drag_var_name.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { 
            _drag_var_name.clear();
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
    }
}

void SFEPanelGraph::OnMessage(const SFEMessage& msg) {
    if (msg.msg.empty()) {
        auto cmd = msg.json_msg["command"].asString();
        if (cmd == "set_cur_graph") {
            _set_graph = msg.json_msg["graph_name"].asString();
        }
        if (cmd == "run_cur_graph") {
            _runing = msg.json_msg["run"].asBool();
            LOG(INFO) << (_runing ? "Runing" : "Stop") << " current graph";
        }
    }
}

} // namespace sfe