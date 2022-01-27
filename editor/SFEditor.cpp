#include "SFEditor.hpp"
#include "SFEPanelMainMenu.hpp"
#include "SFEPanelBp.hpp"
#include "SFEPanelLib.hpp"
#include "SFEPanelDragTip.hpp"
#include "SFEPanelGraph.hpp"
#include "SFEPanelLog.hpp"
#include "SFEPanelUINodes.hpp"
#include "SFEPanelPlot.hpp"
#include "Bp.hpp"

namespace sfe {

bool SFEditor::Init() {
    _panels.emplace_back(std::make_shared<SFEPanelUINodes>());
    _panels.emplace_back(std::make_shared<SFEPanelMainMenu>());
    _panels.emplace_back(std::make_shared<SFEPanelBp>());
    _panels.emplace_back(std::make_shared<SFEPanelLib>());
    _panels.emplace_back(std::make_shared<SFEPanelDragTip>());
    _panels.emplace_back(std::make_shared<SFEPanelGraph>());
    _panels.emplace_back(std::make_shared<SFEPanelLog>());
    _panels.emplace_back(std::make_shared<SFEPanelPlot>());

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
            // boardcast
            if (msgs[j].dst == "all") {
                ProcEditorMessage(msgs[j]);
                for (int k = 0; k < _panels.size(); ++k) {
                    if (_panels[k]->PanelName() == msgs[j].src) {
                        continue;
                    }
                    _panels[k]->RecvMessage(msgs[j]);
                }
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

void SFEditor::ProcEditorMessage(const SFEMessage& msg) {
    if (msg.msg == "show_demo") {
        _show_demo = !_show_demo;
    }
    if (msg.msg.empty()) {
        auto cmd = msg.json_msg["command"];
        if (cmd == "create_new") {
            auto graph_name = msg.json_msg["graph_name"].asString();
            auto graph_type = msg.json_msg["graph_type"].asString() == "mod graph" ? bp::BpNodeType::BP_GRAPH : bp::BpNodeType::BP_GRAPH_EXEC;
            auto g = std::dynamic_pointer_cast<bp::BpGraph>(bp::Bp::Instance().SpawnNode(graph_name, graph_type));
            bp::Bp::Instance().AddEditGraph(graph_name, g);
            bp::Bp::Instance().SetCurEditGraph(g);
            // send to graph
            Json::Value v;
            v["command"] = "set_cur_graph";
            v["graph_name"] = graph_name;
            auto panel = GetPanel("graph");
            panel->RecvMessage({"editor", "graph", "", v});
        } else if (cmd == "spawn_node") {
            auto obj_type = bp::BpNodeType::BP_NONE;
            int contents_type = msg.json_msg["type"].asInt();
            if (contents_type == (int)BpContents::LeafType::EV) {
                obj_type = bp::BpNodeType::BP_NODE_EV;
            } else if (contents_type == (int)BpContents::LeafType::FUNC) {
                obj_type = bp::BpNodeType::BP_NODE_FUNC;
            } else if (contents_type == (int)BpContents::LeafType::VAL) {
                obj_type = bp::BpNodeType::BP_NODE_VAR;
            } else if (contents_type == (int)BpContents::LeafType::BASE) {
                obj_type = bp::BpNodeType::BP_NODE_BASE;
            } else if (contents_type == (int)BpContents::LeafType::USER) {
                obj_type = bp::BpNodeType::BP_NODE_USER;
            } else if (contents_type == (int)BpContents::LeafType::GRAPH) {
                obj_type = bp::BpNodeType::BP_GRAPH;
            }
            if (obj_type == bp::BpNodeType::BP_NONE) {
                LOG(ERROR) << "obj_type is none, return";
                return;
            }
            std::shared_ptr<bp::BpNode> node = nullptr;
            auto g = bp::Bp::Instance().CurEditGraph();
            if (obj_type == bp::BpNodeType::BP_GRAPH && g == nullptr) {
                node = bp::Bp::Instance().SpawnNode(msg.json_msg["node_name"].asString(), obj_type);
                auto new_graph = std::dynamic_pointer_cast<bp::BpGraph>(node);
                bp::Bp::Instance().AddEditGraph(msg.json_msg["node_name"].asString(), new_graph);
                bp::Bp::Instance().SetCurEditGraph(new_graph);
                return;
            } else if (g == nullptr) {
                LOG(WARNING) << "cur edit graph is nullptr";
                return;
            }
            if (obj_type == bp::BpNodeType::BP_NODE_VAR) {
                if (msg.json_msg["node_name"].asString().empty()) {
                    node = bp::Bp::Instance().SpawnVarNode(g, 
                                msg.json_msg["var_name"].asString(),
                                msg.json_msg["is_get"].asBool());
                } else {
                    node = bp::Bp::Instance().SpawnVarNode(g, 
                                msg.json_msg["node_name"].asString(),
                                msg.json_msg["var_name"].asString(),
                                msg.json_msg["is_get"].asBool());
                }
            } else {
                node = bp::Bp::Instance().SpawnNode(msg.json_msg["node_name"].asString(), obj_type);
            }
            if (node == nullptr) {
                LOG(WARNING) << "SpawnNode failed";
                return;
            }
            if (node->GetNodeType() == bp::BpNodeType::BP_NODE_EV) {
                LOG(INFO) << "Create ev node " << node->GetName();
                g->AddEventNode(node);
            } else {
                LOG(INFO) << "Create node " << node->GetName();
                g->AddNode(node);
            }
            // 发送给bp editor消息,设置node的坐标
            Json::Value v;
            v["command"] = "set_node_pos";
            v["node_id"] = node->GetID();
            v["x"] = msg.json_msg["x"];
            v["y"] = msg.json_msg["y"];
            auto panel = GetPanel("bp editor");
            panel->RecvMessage({"editor", "bp editor", "", v});
        } else if (cmd == "open_graph") {
            auto path = msg.json_msg["path"].asString();
            std::shared_ptr<bp::BpGraph> g = nullptr;
            bp::LoadSaveState state = bp::LoadSaveState::OK;
            Json::Value nodes_pos;
            if (bp::LoadSaveState::OK == (state = bp::Bp::Instance().LoadGraph(path, g, nodes_pos))) {
                bp::Bp::Instance().AddEditGraph(g->GetName(), g);
                bp::Bp::Instance().SetCurEditGraph(g);
                // set nodes pos
                Json::Value msg2;
                msg2["command"] = "set_nodes_pos";
                msg2["desc"] = Json::FastWriter().write(nodes_pos);
                auto panel = GetPanel("bp editor");
                panel->RecvMessage({"editor", "bp editor", "", msg2});
            } else {
                LOG(ERROR) << "Load graph " << path << " failed, " << (int)state;
            }
        } else if (cmd == "save_graph_step2") {
            auto path = msg.json_msg["path"].asString();
            auto g = bp::Bp::Instance().CurEditGraph();
            if (g == nullptr) {
                LOG(WARNING) << "cur edit graph is nullptr";
                return;
            }
            bp::LoadSaveState state = bp::LoadSaveState::OK;
            Json::Value nodes_desc;
            Json::Reader reader(Json::Features::strictMode());
            if (!reader.parse(msg.json_msg["nodes_pos"].asString(), nodes_desc)) {
                LOG(ERROR) << "parse nodes desc failed";
                return;
            }
            if (bp::LoadSaveState::OK != (state = bp::Bp::Instance().SaveGraph(path, g, nodes_desc))) {
                LOG(ERROR) << "Save graph " << path << " failed, " << (int)state;
            }
        } else if (cmd == "switch_graph") {
            bp::Bp::Instance().SetCurEditGraph(msg.json_msg["name"].asString());
        }
    }
}

} // namespace sfe