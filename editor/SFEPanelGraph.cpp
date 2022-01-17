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
    ImGui::End();

    if (_runing) {
        bp::Bp::Instance().CurEditGraph()->Run();
    }
}

void SFEPanelGraph::Exit() {

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