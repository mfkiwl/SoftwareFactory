#include "SFEPanelBp.hpp"
#include "Bp.hpp"
#include "BpGraph.hpp"
#include "bpcommon.hpp"
#include "bpflags.hpp"

#define STB_IMAGE_IMPLEMENTATION
extern "C" {
#include "stb/stb_image.h"
}

namespace sfe {

bool SFEPanelBp::Init() {
    std::string title_png_path = bp::FLAGS_assets_dir + "BlueprintBackground.png";
    LoadTexture(title_png_path.c_str());

    ed::Config config;
    _node_editor = ed::CreateEditor(&config);
    ed::SetCurrentEditor(_node_editor);
    ed::GetStyle().NodeRounding = 4;
    ed::NavigateToContent();
    return true;
}

void SFEPanelBp::Update() {
    auto graph = bp::Bp::Instance().CurEditGraph();

    if (!ImGui::Begin(PanelName().c_str(), &_show)) {
        ImGui::End();
        return;
    }

    ed::SetCurrentEditor(_node_editor);
    // double click node
    ed::NodeId n_id = ed::GetDoubleClickedNode();
	std::shared_ptr<bp::BpNode> dc_node = nullptr;
	if (graph != nullptr && (dc_node = graph->GetNode((int)n_id.Get())) != nullptr) {
        _is_doubleclick_node = true;
		_doubleclick_node = dc_node;
	}
    // begin node editor
    ed::Begin("Node editor");
    auto cursorTopLeft = ImGui::GetCursorScreenPos();
    util::BlueprintNodeBuilder builder(reinterpret_cast<ImTextureID>(_bg_texture.TextureID), _bg_texture.Width, _bg_texture.Height);
    if (graph != nullptr) {
        // ev nodes
        for (auto& p : graph->GetEvNodes()) {
            auto n = std::dynamic_pointer_cast<bp::BpNode>(p.second);
            ShowNode(builder, n);
        }
		
		// nodes
		auto& nodes = graph->GetNodes();
		for (auto node : nodes) {
			ShowNode(builder, node);
		}
    
        // links
        auto& links = graph->GetLinks();
        for (auto it = links.begin(); it != links.end(); ++it)
            ed::Link((*it).ID, (*it).StartPinID, (*it).EndPinID, ImColor((*it).Color[0], (*it).Color[1], (*it).Color[2], (*it).Color[3]), 2.0f);
        // links flow
        if (!_flow_links.empty()) {
             for (int i = 0; i < _flow_links.size(); ++i) {
                ed::Flow(_flow_links[i]);
            }
            _flow_links.clear();
        }
        // create/delete
        NodeLinkCreate(graph);
        NodeLinkDelete(graph);

        ShowNodeInfo();
    }
	ed::End();
    
    // graph input/output setting
    ShowPinSetting();

    // node attr
    if (_is_doubleclick_node) {
        _is_doubleclick_node = false;
        OnDoubleclickNode();
    }
    ShowVarNodeAttr();
    ImGui::End();
}

void SFEPanelBp::NodeLinkCreate(std::shared_ptr<bp::BpGraph>& graph) {
    if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
        auto showLabel = [](const char* label, ImColor color)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
            auto size = ImGui::CalcTextSize(label);

            auto padding = ImGui::GetStyle().FramePadding;
            auto spacing = ImGui::GetStyle().ItemSpacing;

            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

            auto rectMin = ImGui::GetCursorScreenPos() - padding;
            auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
            ImGui::TextUnformatted(label);
        };

        ed::PinId startPinId = 0, endPinId = 0;
        bp::BpPin* new_link_pin = nullptr;
        if (ed::QueryNewLink(&startPinId, &endPinId)) {
            auto startPin = graph->GetPin((int)startPinId.Get());
            auto endPin = graph->GetPin((int)endPinId.Get());

            new_link_pin = startPin ? startPin : endPin;

            if (startPin->GetPinKind() == bp::BpPinKind::BP_INPUT) {
                std::swap(startPin, endPin);
                std::swap(startPinId, endPinId);
            }

            if (startPin && endPin) {
                if (endPin == startPin) {
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (endPin->GetPinKind() == startPin->GetPinKind()) {
                    showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (endPin->GetObj() == startPin->GetObj()) {
                    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                } else if (!startPin->IsSameType(endPin)) {
                    showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                } else if (!CanCreateLink(startPin, endPin)) {
                    showLabel("x linked", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                } else {
                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                    if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                        Json::Value v;
                        v["command"] = "create_link";
                        v["start_pin_id"] = startPin->ID;
                        v["end_pin_id"] = endPin->ID;
                        ImColor c = GetIconColor(*startPin);
                        v["color"].append(c.Value.x);
                        v["color"].append(c.Value.y);
                        v["color"].append(c.Value.z);
                        v["color"].append(c.Value.w);
                        SendMessage("editor", v);
                    }
                }
            }
        }

        ed::PinId pinId = 0;
        if (ed::QueryNewNode(&pinId)) {
            new_link_pin = graph->GetPin((int)pinId.Get());
            if (new_link_pin)
                showLabel("+ Create Node", ImColor(32, 45, 32, 180));

            if (ed::AcceptNewItem()) {
                // m_create_new_node = true;
                // m_new_node_link_pin = graph->GetPin((int)pinId.Get());
                ed::Suspend();
                ImGui::OpenPopup("Create New Node");
                ed::Resume();
            }
        }
    }
    ed::EndCreate();
}

void SFEPanelBp::NodeLinkDelete(std::shared_ptr<bp::BpGraph>& graph) {
    if (ed::BeginDelete()) {
        ed::LinkId linkId = 0;
        while (ed::QueryDeletedLink(&linkId)) {
            if (ed::AcceptDeletedItem()) {
                auto id = std::find_if(graph->GetLinks().begin(), graph->GetLinks().end(), [linkId](auto& link) { return link.ID == (int)linkId.Get(); });
                if (id != graph->GetLinks().end()) {
                    Json::Value v;
                    v["command"] = "del_link";
                    v["id"] = id->ID;
                    SendMessage("editor", v);
                }
            }
        }

        ed::NodeId nodeId = 0;
        while (ed::QueryDeletedNode(&nodeId)) {
            if (ed::AcceptDeletedItem()) {
                auto& nodes = graph->GetNodes();
                auto id = std::find_if(nodes.begin(), nodes.end(), [nodeId](auto& node) { return node->GetID() == (int)nodeId.Get(); });
                if (id != nodes.end()) {
                    Json::Value v;
                    v["command"] = "del_node";
                    v["id"] = (*id)->GetID();
                    SendMessage("editor", v);
                }

                auto& ev_nodes = graph->GetEvNodes();
                auto p = std::find_if(ev_nodes.begin(), ev_nodes.end(), [nodeId](auto node) { return node.second->GetID() == (int)nodeId.Get(); });
                if (p != ev_nodes.end()) {
                    Json::Value v;
                    v["command"] = "del_evnode";
                    v["name"] = (*p).first;
                    SendMessage("editor", v);
                }
            }
        }
    }
    ed::EndDelete();
}

void SFEPanelBp::Exit() {
    ed::DestroyEditor(_node_editor);
    _node_editor = nullptr;
}

void SFEPanelBp::ShowVarNodeAttr() {
    if (ImGui::BeginPopupModal("node attr", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        // Show node attr
        ImGui::TextUnformatted("variable attr:");
        ImGui::InputTextMultiline("", _doubleclick_node_edit_attr, sizeof(_doubleclick_node_edit_attr));
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            LOG(INFO) << "set var attr: " << std::string(_doubleclick_node_edit_attr);
            _doubleclick_node->GetPins(bp::BpPinKind::BP_OUTPUT)[0].SetValue(std::string(_doubleclick_node_edit_attr));
            _doubleclick_node = nullptr;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { 
            _doubleclick_node = nullptr;
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
    }
}

void SFEPanelBp::ShowPinSetting() {
	if (_show_graph_setting) {
        _show_graph_setting = false;
		ImGui::OpenPopup("Graph pin setting");
    }

	if (ImGui::BeginPopupModal("Graph pin setting", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char buf[64] = "";
        if (_graph_pin_info["command"] == "add_graph_pin") {
            ImGui::InputText("variable type", buf, 64, ImGuiInputTextFlags_CharsNoBlank);
        }
        if (ImGui::Button("OK", ImVec2(120, 0))) { 
            _graph_pin_info["var_type"] = buf;
            SendMessage("editor", _graph_pin_info);
            ImGui::CloseCurrentPopup();
            memset(buf, 0, sizeof(buf));
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
    }
}

void SFEPanelBp::ShowNode(util::BlueprintNodeBuilder& builder, std::shared_ptr<bp::BpNode>& node) {
	if (node->GetNodeStyle() != bp::BpNodeStyle::BP_BLUPRINT
		&& node->GetNodeStyle() != bp::BpNodeStyle::BP_SIMPLE)
		return;

	bool is_simple = (node->GetNodeStyle() == bp::BpNodeStyle::BP_SIMPLE);
	builder.Begin(node->GetID());
	if (!is_simple) {
		builder.Header(GetNodeKindColor(node->GetNodeType()));
		ImGui::Spring(0);
		ImGui::TextUnformatted(node->GetName().c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 24));
		ImGui::Spring(0);
		builder.EndHeader();
	}
	// inputs
	auto& inputs = node->GetPins(bp::BpPinKind::BP_INPUT);
	for (auto& input : inputs) {
		auto alpha = ImGui::GetStyle().Alpha;
		// if (m_new_link_pin && !CanCreateLink(m_new_link_pin, &input) && &input != m_new_link_pin)
		// 	alpha = alpha * (48.0f / 255.0f);
		builder.Input(input.ID);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
		DrawPinIcon(input, (int)(alpha * 255));
		ImGui::Spring(0);
		if (!input.GetName().empty()) {
			ImGui::TextUnformatted(input.GetName().c_str());
			ImGui::Spring(0);
		}
		ImGui::PopStyleVar();
		builder.EndInput();
	}
    if (node->GetNodeType() == bp::BpNodeType::BP_GRAPH_OUTPUT) {
        if (ImGui::Button(" + ")) {
            _show_graph_setting = true;
            _graph_pin_info.clear();
            _graph_pin_info["command"] = "add_graph_pin";
            _graph_pin_info["node_type"] = (int)bp::BpNodeType::BP_GRAPH_OUTPUT;
        } 
        // ImGui::SameLine();
        if (ImGui::Button(" - ")) {
            _show_graph_setting = true;
            _graph_pin_info.clear();
            _graph_pin_info["command"] = "del_graph_pin";
            _graph_pin_info["node_id"] = node->GetID();
        }
    }

	if (is_simple) {
		builder.Middle();
		ImGui::Spring(1, 0);
		ImGui::TextUnformatted(node->GetName().c_str());
		ImGui::Spring(1, 0);
	}
	// outputs
    auto& outputs = node->GetPins(bp::BpPinKind::BP_OUTPUT);
	for (auto& output : outputs) {
		auto alpha = ImGui::GetStyle().Alpha;
		// if (m_new_link_pin && !CanCreateLink(m_new_link_pin, &output) && &output != m_new_link_pin)
		// 	alpha = alpha * (48.0f / 255.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
		builder.Output(output.ID);
		if (!output.GetName().empty()) {
			ImGui::Spring(0);
			ImGui::TextUnformatted(output.GetName().c_str());
		}
		ImGui::Spring(0);
		DrawPinIcon(output, (int)(alpha * 255));
		ImGui::PopStyleVar();
		builder.EndOutput();
	}
    // add pin icon
    if (node->GetNodeType() == bp::BpNodeType::BP_GRAPH_INPUT) {
        if (ImGui::Button(" + ")) {
            _show_graph_setting = true;
            _graph_pin_info.clear();
            _graph_pin_info["command"] = "add_graph_pin";
            _graph_pin_info["node_type"] = (int)bp::BpNodeType::BP_GRAPH_INPUT;
        }
        ImGui::SameLine();
        if (ImGui::Button(" - ")) {
            _show_graph_setting = true;
            _graph_pin_info.clear();
            _graph_pin_info["command"] = "del_graph_pin";
            _graph_pin_info["node_id"] = node->GetID();
        }
    }

	builder.End();
}

void SFEPanelBp::ShowNodeInfo()
{
	auto openPopupPosition = ImGui::GetMousePos();
	ed::Suspend();
	if (ed::ShowNodeContextMenu(&_right_click_nodeid)) {
		ImGui::OpenPopup("Node Context Menu");
    } else if (ed::ShowPinContextMenu(&_right_click_pinid)) {
		ImGui::OpenPopup("Pin Context Menu");
    } else if (ed::ShowLinkContextMenu(&_right_click_linkid)) {
		ImGui::OpenPopup("Link Context Menu");
    }
	ed::Resume();

	ed::Suspend();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    auto g = bp::Bp::Instance().CurEditGraph();
	// 显示节点信息
	if (ImGui::BeginPopup("Node Context Menu")) {
		auto node = g->GetNode((int)_right_click_nodeid.Get());
		ImGui::TextUnformatted("Node Context Menu");
		ImGui::Separator();
		if (node) {
			ImGui::Text("ID: %d", node->GetID());
			ImGui::Text("Type: %s", node->GetNodeStyle() == bp::BpNodeStyle::BP_BLUPRINT ? "Blueprint" : "Simple");
			ImGui::Text("Inputs: %d", (int)node->GetPins(bp::BpPinKind::BP_INPUT).size());
			ImGui::Text("Outputs: %d", (int)node->GetPins(bp::BpPinKind::BP_OUTPUT).size());
		} else {
			ImGui::Text("Unknown node: %p", _right_click_nodeid.AsPointer());
        }
		ImGui::EndPopup();
	}
	// 显示针脚信息
	if (ImGui::BeginPopup("Pin Context Menu")) {
		auto pin = g->GetPin((int)_right_click_pinid.Get());
		ImGui::TextUnformatted("Pin Context Menu");
		ImGui::Separator();
		if (pin) {
			ImGui::Text("ID: %d", pin->ID);
			if (pin->GetObj()) {
				ImGui::Text("Node: %d", pin->GetObj()->GetID());
            } else {
				ImGui::Text("Node: %s", "<none>");
            }
		} else {
			ImGui::Text("Unknown pin: %p", _right_click_pinid.AsPointer());
        }
		ImGui::EndPopup();
	}
	// 显示连线信息
	if (ImGui::BeginPopup("Link Context Menu")) {
		auto link = g->GetLink((int)_right_click_linkid.Get());
		ImGui::TextUnformatted("Link Context Menu");
		ImGui::Separator();
		if (link->ID != 0) {
			ImGui::Text("ID: %d", link->ID);
			ImGui::Text("From: %d", link->StartPinID);
			ImGui::Text("To: %d", link->EndPinID);
		} else {
			ImGui::Text("Unknown link: %p", _right_click_linkid.AsPointer());
        }
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
	ed::Resume();
}

void SFEPanelBp::SetNodesPos(const std::string& desc) {
    auto v = bp::BpCommon::Str2Json(desc);
    if (v == Json::Value::null) {
        LOG(ERROR) << "Parse node desc failed, " << desc;
        return;
    }
    Json::Value::Members mem = v.getMemberNames();
	Json::Value::Members::iterator it;
	for (it = mem.begin(); it != mem.end(); it++) {
        auto id = std::stoi(*it);
        auto pos = v[(*it)]["pos"];
        ed::SetNodePosition((ed::NodeId)id, ImVec2(pos[0].asInt(), pos[1].asInt()));
    }
}

void SFEPanelBp::DrawPinIcon(bp::BpPin& pin, int alpha) {
	IconType iconType = iconType = IconType::Circle;
	ImColor  color = GetIconColor(pin);
	color.Value.w = alpha / 255.0f;
	switch (pin.GetPinType()) {
	case bp::BpPinType::BP_FLOW:     iconType = IconType::Flow;   break;
	case bp::BpPinType::BP_VALUE:    iconType = IconType::Circle; break;
	}
	ax::Widgets::Icon(ImVec2(24, 24), iconType, pin.IsLinked(), color, ImColor(32, 32, 32, alpha));
}

bool SFEPanelBp::CanCreateLink(bp::BpPin* a, bp::BpPin* b) {
	if (!a || !b || a == b 
		|| a->GetPinKind() == b->GetPinKind() 
		|| a->IsLinked()
		|| b->IsLinked()
		|| a->GetObj() == b->GetObj() 
		|| !a->IsSameType(b)) {
		return false;
    }
	return true;
}

ImColor SFEPanelBp::GetIconColor(bp::BpPin& pin) {
	bp::BpPinType type = pin.GetPinType();
	switch (type)
	{
	case bp::BpPinType::BP_FLOW:      
        return ImColor(255, 255, 255);
	}
    auto var_type = pin.GetVarType();
    auto color = bp::Bp::Instance().GetVarColor(var_type);
	return ImColor(color);
}

ImColor SFEPanelBp::GetNodeKindColor(bp::BpNodeType kind) {
	switch (kind) {
	default:
	case bp::BpNodeType::BP_NODE_EV:     return ImColor(255, 128, 128);
	case bp::BpNodeType::BP_NODE_VAR:	return ImColor(51, 150, 215);
	case bp::BpNodeType::BP_NODE_FUNC:	return ImColor(255, 255, 255);
	}
}

ImTextureID SFEPanelBp::LoadTexture(const char* path) {
    int width = 0, height = 0, component = 0;
    if (auto data = stbi_load(path, &width, &height, &component, 4)) {
        auto texture = CreateTexture(data, width, height);
        stbi_image_free(data);
        return texture;
    } else {
        return nullptr;
    }
}

ImTextureID SFEPanelBp::CreateTexture(const void* data, int width, int height) {
    ImTexture& texture = _bg_texture;

    // Upload texture to graphics system
    GLint last_texture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &texture.TextureID);
    glBindTexture(GL_TEXTURE_2D, texture.TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, last_texture);

    texture.Width  = width;
    texture.Height = height;

    return reinterpret_cast<ImTextureID>(texture.TextureID);
}

void SFEPanelBp::OnDoubleclickNode() {
    switch (_doubleclick_node->GetNodeType()) {
    case bp::BpNodeType::BP_NODE_VAR: {
        auto& pins = _doubleclick_node->GetPins(bp::BpPinKind::BP_OUTPUT);
        if (pins.size() > 0){
            // FIXME, only show one variable
            _doubleclick_node_attr.clear();
            bp::JsonPbConvert::PbMsg2JsonStr(*pins[0].Get<pb_msg_t>(), _doubleclick_node_attr);
            LOG(INFO) << "get var attr: " << _doubleclick_node_attr;
            strcpy(_doubleclick_node_edit_attr, _doubleclick_node_attr.c_str());
            ImGui::OpenPopup("node attr");
        }
    }
    break;
    case bp::BpNodeType::BP_GRAPH: {
        // FIXME: 添加编辑图未添加到命令模式
        auto g = std::dynamic_pointer_cast<bp::BpGraph>(_doubleclick_node);
        bp::Bp::Instance().AddEditGraph(g->GetName(), g);
        Json::Value v;
        v["command"] = "switch_graph";
        v["name"] = g->GetName();
        SendMessage("editor", v);
    }
    break;
    }
}

void SFEPanelBp::OnMessage(const SFEMessage& msg) {
    if (msg.json_msg.isNull()) {
        return;
    }
    auto jmsg = msg.json_msg;
    auto cmd = msg.json_msg["command"].asString();
    if (cmd == "set_node_pos") {
        auto mouse_pos = ImVec2(msg.json_msg["x"].asInt(), msg.json_msg["y"].asInt());
        auto canvas_pos = ed::ScreenToCanvas(mouse_pos);
        ed::SetNodePosition((ed::NodeId)msg.json_msg["node_id"].asInt(), 
            canvas_pos
            );
    } else if (cmd == "set_nodes_pos") {
        SetNodesPos(msg.json_msg["desc"].asString());
    } else if (cmd == "save_graph_step1") {
        auto g = bp::Bp::Instance().CurEditGraph();
        if (g == nullptr) {
            LOG(WARNING) << "cur edit graph is nullptr";
            return;
        }
        auto& ev_nodes = g->GetEvNodes();
        auto& nodes = g->GetNodes();
        Json::Value root;
        for (const auto& it : ev_nodes) {
            Json::Value v;
            auto pos = ed::GetNodePosition((ed::NodeId(it.second->GetID())));
            v["pos"].append(pos.x);
            v["pos"].append(pos.y);
            root[std::to_string(it.second->GetID())] = v;
        }
        for (const auto& it : nodes) {
            Json::Value v;
            auto pos = ed::GetNodePosition((ed::NodeId(it->GetID())));
            v["pos"].append(pos.x);
            v["pos"].append(pos.y);
            root[std::to_string(it->GetID())] = v;
        }
        Json::Value msg2;
        msg2["command"] = "save_graph_step2";
        msg2["path"] = msg.json_msg["path"].asString();
        msg2["nodes_pos"] = bp::BpCommon::Json2Str(root);
        SendMessage("editor", msg2);
    } else if (cmd == "move_node_to_center") {
        ed::SelectNode(jmsg["id"].asInt());
        ed::NavigateToSelection();
    } else if (cmd == "debug_cur_graph" && jmsg["type"].asString() == "resp") {
        if (jmsg["stage"] == "continue") {
            auto flow_links = jmsg["flow_links"];
            for (auto it = flow_links.begin(); it != flow_links.end(); ++it) {
                _flow_links.push_back((*it).asInt());
            }
        }
    }
}

} // namespace sfe