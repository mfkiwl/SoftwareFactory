#include "SFEditor.hpp"
#include "SFEPanelMainMenu.hpp"
#include "SFEPanelBp.hpp"
#include "SFEPanelLib.hpp"
#include "SFEPanelDragTip.hpp"
#include "SFEPanelGraph.hpp"
#include "SFEPanelLog.hpp"
#include "SFEPanelUINodes.hpp"
#include "SFEPanelPlot.hpp"
#include "bpcommon.hpp"
#include "Bp.hpp"
#include "bplog/bplog.hpp"
#include "bpflags.hpp"
#include "SFEParams.hpp"

namespace sfe {

bool SFEditor::Init() {
    bp::InitLogging("SoftwareFactory");

    auto log_panel = std::make_shared<SFEPanelLog>();
    bp::RegisterWriteCallback(std::bind(&SFEPanelLog::AddLogCB, log_panel.get(), std::placeholders::_1));
    SFEPanel::RegPanel("log", log_panel);

    SFEPanel::RegPanel("uinodes", std::make_shared<SFEPanelUINodes>());
    SFEPanel::RegPanel("mainmenu", std::make_shared<SFEPanelMainMenu>());
    SFEPanel::RegPanel("bp editor", std::make_shared<SFEPanelBp>());
    SFEPanel::RegPanel("lib", std::make_shared<SFEPanelLib>());
    SFEPanel::RegPanel("drag tip", std::make_shared<SFEPanelDragTip>());
    SFEPanel::RegPanel("graph", std::make_shared<SFEPanelGraph>());
    SFEPanel::RegPanel("plot", std::make_shared<SFEPanelPlot>());

    _pluglib = std::make_unique<SFEPlugLib>();
    _pluglib->Init(bp::FLAGS_plug_conf_dir);

    _ratectrl.reset();
    return true;
}

void SFEditor::ProcEvent(const SDL_Event event) {
    const auto& panels = SFEPanel::GetPanels();
    for (auto it = panels.begin(); it != panels.end(); ++it) {
        (it->second)->ProcEvent(event);
    }
}

void SFEditor::RunGraph() {
    if (!_runing) {
        return;
    }
    auto g = bp::Bp::Instance().CurEditGraph();
    if (g == nullptr) {
        return;
    }
    if (g->GetNodeType() == bp::BpNodeType::BP_GRAPH) {
        g->ClearFlag();
        g->Logic();
    } else {
        if (_run_state == 1) {
            g->RunNextEventBeign();
            _run_state = 2;
        } else if (bp::BpNodeRunState::BP_RUN_FINISH == g->RunNextEvent()) {
            Json::Value v;
            v["command"] = "run_cur_graph";
            v["run"] = false;
            SFEPanel::SendMessage(_name, "all", v);
        }
    }
}

void SFEditor::Update() {
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! 
    // You can browse its code to learn more about Dear ImGui!).
    if (_show_demo) {
        ImGui::ShowDemoWindow(&_show_demo);
    }
    const auto& panels = SFEPanel::GetPanels();
    for (auto it = panels.begin(); it != panels.end(); ++it) {
        (it->second)->GlobalUpdate();
    }
    RunGraph();
}

void SFEditor::DispatchMessage() {
    // 只需要处理属于editor的消息, 其它不属于panel的消息都丢弃掉
    const auto& msgs = SFEPanel::GetDispatchMessage();
    for (int i = 0; i < msgs.size(); ++i) {
        // boardcast
        if (msgs[i].dst == "all") {
            _recv_que.emplace_back(msgs[i]);
            auto& panels = SFEPanel::GetPanels();
            for (auto it = panels.begin(); it != panels.end(); ++it) {
                if (it->second->PanelName() == msgs[i].src) {
                    continue;
                }
                it->second->RecvMessage(msgs[i]);
            }
            LOG(INFO) << "boardcast msg: " << msgs[i].Print();
            continue;
        }
        // dispatch
        auto dst = msgs[i].dst;
        if (dst == _name) {
            _recv_que.emplace_back(msgs[i]);
        } else {
            LOG(WARNING) << "Unknown msg " << msgs[i].Print();
        }
    }
    SFEPanel::ClearDispathMessage();
}

void SFEditor::ProcMessage() {
    const auto& panels = SFEPanel::GetPanels();
    for (auto it = panels.begin(); it != panels.end(); ++it) {
        (it->second)->ProcMessage();
    }
    for (int i = 0; i < _recv_que.size(); ++i) {
        ProcEditorMessage(_recv_que[i]);
    }
    _recv_que.clear();
}

void SFEditor::Exit() {
    const auto& panels = SFEPanel::GetPanels();
    for (auto it = panels.begin(); it != panels.end(); ++it) {
        (it->second)->GlobalExit();
    }
    SFEPanel::ClearPanels();
    SFEParams::Instance().SaveParams();
}

const std::shared_ptr<SFEPanel> SFEditor::GetPanel(const std::string& name) {
    return SFEPanel::GetPanel(name);
}

void SFEditor::ProcEditorMessage(const SFEMessage& msg) {
    if (msg.msg == "show_demo") {
        _show_demo = !_show_demo;
    }
    if (msg.json_msg.isNull()) {
        return;
    }
    auto jmsg = msg.json_msg;
    auto cmd = jmsg["command"];
    if (cmd == "create_new") {
        auto graph_name = jmsg["graph_name"].asString();
        auto graph_type = jmsg["graph_type"].asString() == "mod graph" ? bp::BpNodeType::BP_GRAPH : bp::BpNodeType::BP_GRAPH_EXEC;
        if (bp::Bp::Instance().HasEditGraph(graph_name)) {
            LOG(WARNING) << graph_name << " graph has exist";
            return;
        }
        auto g = std::dynamic_pointer_cast<bp::BpGraph>(bp::Bp::Instance().SpawnNode(graph_name, graph_type));
        bp::Bp::Instance().AddEditGraph(graph_name, g);
        bp::Bp::Instance().SetCurEditGraph(g);
        // send to graph
        Json::Value v;
        v["command"] = "set_cur_graph";
        v["graph_name"] = graph_name;
        SFEPanel::SendMessage(_name, "graph", v);
    } else if (cmd == "spawn_node") {
        auto obj_type = bp::BpNodeType::BP_NONE;
        int contents_type = jmsg["type"].asInt();
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
            node = bp::Bp::Instance().SpawnNode(jmsg["node_name"].asString(), obj_type);
            auto new_graph = std::dynamic_pointer_cast<bp::BpGraph>(node);
            bp::Bp::Instance().AddEditGraph(jmsg["node_name"].asString(), new_graph);
            bp::Bp::Instance().SetCurEditGraph(new_graph);
            Json::Value v;
            v["command"] = "set_nodes_pos";
            v["desc"] = bp::BpCommon::Json2Str(new_graph->GetNodesPos());
            SFEPanel::SendMessage(_name, "bp editor", v);
            return;
        } else if (g == nullptr) {
            LOG(WARNING) << "cur edit graph is nullptr";
            return;
        }
        if (obj_type == bp::BpNodeType::BP_NODE_VAR) {
            if (jmsg["node_name"].asString().empty()) {
                node = bp::Bp::Instance().SpawnVarNode(g, 
                            jmsg["var_name"].asString(),
                            jmsg["is_get"].asBool());
            } else {
                node = bp::Bp::Instance().SpawnVarNode(g, 
                            jmsg["node_name"].asString(),
                            jmsg["var_name"].asString(),
                            jmsg["is_get"].asBool());
            }
        } else {
            node = bp::Bp::Instance().SpawnNode(jmsg["node_name"].asString(), obj_type);
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
        v["x"] = jmsg["x"];
        v["y"] = jmsg["y"];
        SFEPanel::SendMessage(_name, "bp editor", v);
    } else if (cmd == "open_graph") {
        auto path = jmsg["path"].asString();
        std::shared_ptr<bp::BpGraph> g = nullptr;
        bp::LoadSaveState state = bp::LoadSaveState::OK;
        Json::Value nodes_pos;
        if (bp::LoadSaveState::OK == (state = bp::Bp::Instance().LoadGraph(path, g, nodes_pos))) {
            bp::Bp::Instance().AddEditGraph(g->GetName(), g);
            bp::Bp::Instance().SetCurEditGraph(g);
            // set nodes pos
            Json::Value msg2;
            msg2["command"] = "set_nodes_pos";
            msg2["desc"] = bp::BpCommon::Json2Str(nodes_pos);;
            SFEPanel::SendMessage(_name, "bp editor", msg2);
        } else {
            LOG(ERROR) << "Load graph " << path << " failed, " << (int)state;
        }
    } else if (cmd == "save_graph_step2") {
        auto path = jmsg["path"].asString();
        auto g = bp::Bp::Instance().CurEditGraph();
        if (g == nullptr) {
            LOG(WARNING) << "cur edit graph is nullptr";
            return;
        }
        bp::LoadSaveState state = bp::LoadSaveState::OK;

        auto nodes_desc = bp::BpCommon::Str2Json(jmsg["nodes_pos"].asString());
        if (nodes_desc == Json::Value::null) {
            LOG(ERROR) << "parse nodes desc failed";
            return;
        }
        if (bp::LoadSaveState::OK != (state = bp::Bp::Instance().SaveGraph(path, g, nodes_desc))) {
            LOG(ERROR) << "Save graph " << path << " failed, " << (int)state;
        }
    } else if (cmd == "switch_graph") {
        bp::Bp::Instance().SetCurEditGraph(jmsg["name"].asString());
        auto g = bp::Bp::Instance().CurEditGraph();
        Json::Value v;
        v["command"] = "set_nodes_pos";
        v["desc"] = bp::BpCommon::Json2Str(g->GetNodesPos());
        SFEPanel::SendMessage(_name, "bp editor", v);

        v.clear();
        v["command"] = "set_cur_graph";
        v["graph_name"] = jmsg["name"].asString();
        SFEPanel::SendMessage(_name, "graph", v);
    } else if (cmd == "del_node") {
        auto g = bp::Bp::Instance().CurEditGraph();
        g->DelNode(jmsg["id"].asInt());
    } else if (cmd == "del_evnode") {
        auto g = bp::Bp::Instance().CurEditGraph();
        g->DelEventNode(jmsg["name"].asString());
    } else if (cmd == "del_link") {
        auto g = bp::Bp::Instance().CurEditGraph();
        g->DelLink(jmsg["id"].asInt());
    } else if (cmd == "create_link") {
        auto g = bp::Bp::Instance().CurEditGraph();
        auto start_pin = g->SearchPin(jmsg["start_pin_id"].asInt());
        auto end_pin = g->SearchPin(jmsg["end_pin_id"].asInt());
        g->AddLink(*start_pin, *end_pin);
        g->GetLinks().back().SetColor(jmsg["color"][0].asFloat(), jmsg["color"][1].asFloat(), jmsg["color"][2].asFloat(), jmsg["color"][3].asFloat());
    } else if (cmd == "add_graph_pin") {
        auto g = bp::Bp::Instance().CurEditGraph();
        auto var = bp::Bp::Instance().CreateVariable(jmsg["var_type"].asString());
        g->AddModGraphPin(var.GetType(), (bp::BpNodeType)jmsg["node_type"].asInt(), var);
    } else if (cmd == "del_graph_pin") {
        auto g = bp::Bp::Instance().CurEditGraph();
        g->DelModGraphPin(jmsg["node_id"].asInt());
    } else if (cmd == "run_cur_graph") {
        bool is_run = msg.json_msg["run"].asBool();
        _runing = is_run;
        _run_state = 1;
        LOG(INFO) << (_runing ? "Runing" : "Stop") << " current graph";
    }
}

} // namespace sfe