#include "SFEPanelBp.hpp"
#include "Bp.hpp"
#include "BpGraph.hpp"

#define STB_IMAGE_IMPLEMENTATION
extern "C" {
#include "stb/stb_image.h"
}

namespace sfe {

bool SFEPanelBp::Init() {
    SetPanelName("bp editor");
    LoadTexture("../assets/BlueprintBackground.png");

    ed::Config config;
    _node_editor = ed::CreateEditor(&config);
    ed::SetCurrentEditor(_node_editor);
    ed::GetStyle().NodeRounding = 6;
    ed::NavigateToContent();
    return true;
}

void SFEPanelBp::OnMessage(const SFEMessage& msg) {
    if (msg.msg == "spawnnode") {
        auto g = bp::Bp::Instance().CurEditGraph();
        if (g == nullptr) {
            LOG(WARNING) << "cur edit graph is nullptr";
            return;
        }
        auto node = bp::Bp::Instance().SpawnNode("bpmath.add_int");
        if (node == nullptr) {
            LOG(WARNING) << "SpawnNode failed";
            return;
        }
        g->AddNode(node);
        {
            auto mouse_pos = ImGui::GetMousePos();
	        auto canvas_pos = ed::ScreenToCanvas(mouse_pos);
            ed::SetNodePosition((ed::NodeId)node->GetID(), canvas_pos);
        }
    }
    if (msg.msg.empty()) {
        if (msg.json_msg["command"].asString() == "set_node_pos") {
            auto mouse_pos = ImVec2(msg.json_msg["x"].asInt(), msg.json_msg["y"].asInt());
	        auto canvas_pos = ed::ScreenToCanvas(mouse_pos);
            ed::SetNodePosition((ed::NodeId)msg.json_msg["node_id"].asInt(), 
                canvas_pos
                );
        }
    }
}

void SFEPanelBp::Update() {
    ImGui::Begin(PanelName().c_str());

    ed::SetCurrentEditor(_node_editor);
    ed::Begin("Node editor");
    auto cursorTopLeft = ImGui::GetCursorScreenPos();
    util::BlueprintNodeBuilder builder(reinterpret_cast<ImTextureID>(_bg_texture.TextureID), _bg_texture.Width, _bg_texture.Height);
    auto graph = bp::Bp::Instance().CurEditGraph();
    if (graph != nullptr) {
        // ev nodes
        for (auto& p : graph->GetEvNodes()) {
            ShowNode(builder, p.second);
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
    
        // create/delete
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
                auto startPin = graph->SearchPin((int)startPinId.Get());
                auto endPin = graph->SearchPin((int)endPinId.Get());

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
                            graph->AddLink(*startPin, *endPin);
                            ImColor c = GetIconColor(*startPin);
                            graph->GetLinks().back().SetColor(c.Value.x, c.Value.y, c.Value.z, c.Value.w);
                        }
                    }
                }
            }

            ed::PinId pinId = 0;
            if (ed::QueryNewNode(&pinId)) {
                new_link_pin = graph->SearchPin((int)pinId.Get());
                if (new_link_pin)
                    showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                if (ed::AcceptNewItem()) {
                    // m_create_new_node = true;
                    // m_new_node_link_pin = graph->SearchPin((int)pinId.Get());
                    ed::Suspend();
                    ImGui::OpenPopup("Create New Node");
                    ed::Resume();
                }
            }
        }
        ed::EndCreate();

        if (ed::BeginDelete()) {
            ed::LinkId linkId = 0;
            while (ed::QueryDeletedLink(&linkId)) {
                if (ed::AcceptDeletedItem()) {
                    auto id = std::find_if(graph->GetLinks().begin(), graph->GetLinks().end(), [linkId](auto& link) { return link.ID == (int)linkId.Get(); });
                    if (id != graph->GetLinks().end()) {
                        graph->DelLink(id->ID);
                    }
                }
            }

            ed::NodeId nodeId = 0;
            while (ed::QueryDeletedNode(&nodeId)) {
                if (ed::AcceptDeletedItem()) {
                    auto& nodes = graph->GetNodes();
                    auto id = std::find_if(nodes.begin(), nodes.end(), [nodeId](auto node) { return node->GetID() == (int)nodeId.Get(); });
                    if (id != nodes.end()) {
                        graph->DelNode(*id);
                    }

                    auto& ev_nodes = graph->GetEvNodes();
                    auto p = std::find_if(ev_nodes.begin(), ev_nodes.end(), [nodeId](auto node) { return node.second->GetID() == (int)nodeId.Get(); });
                    if (p != ev_nodes.end()) {
                        graph->DelEventNode((*p).first);
                    }
                }
            }
        }
        ed::EndDelete();
    }
	ed::End();
    ImGui::End();
}

void SFEPanelBp::Exit() {
    ed::DestroyEditor(_node_editor);
    _node_editor = nullptr;
}

void SFEPanelBp::ShowNode(util::BlueprintNodeBuilder& builder, std::shared_ptr<bp::BpObj>& node) {
	if (node->GetNodeType() != bp::BpNodeType::BP_BLUPRINT
		&& node->GetNodeType() != bp::BpNodeType::BP_SIMPLE)
		return;

	bool is_simple = (node->GetNodeType() == bp::BpNodeType::BP_SIMPLE);
	builder.Begin(node->GetID());
	if (!is_simple) {
		builder.Header(GetNodeKindColor(node->GetObjType()));
		ImGui::Spring(0);
		ImGui::TextUnformatted(node->GetName().c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 28));
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
	builder.End();
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
	return ImColor(51, 150, 215); // object
}

ImColor SFEPanelBp::GetNodeKindColor(bp::BpObjType kind) {
	switch (kind) {
	default:
	case bp::BpObjType::BP_NODE_EV:     return ImColor(255, 128, 128);
	case bp::BpObjType::BP_NODE_VAR:	return ImColor(51, 150, 215);
	case bp::BpObjType::BP_NODE_NORMAL:	return ImColor(255, 255, 255);
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

} // namespace sfe