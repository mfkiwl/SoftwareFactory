#include "SFEPanelLib.hpp"
#include "Bp.hpp"

namespace sfe {

bool SFEPanelLib::Init() {
    SetPanelName("lib");
    return true;
}

void SFEPanelLib::Update() {
    ImGui::Begin(PanelName().c_str());
    auto root = bp::Bp::Instance().GetContents();
    auto children = root->GetChildren();
    for (int i = 0; i < children.size(); ++i) {
        ShowLib(children[i]);
    }
    ImGui::End();
}

void SFEPanelLib::Exit() {

}

void SFEPanelLib::ShowLib(std::shared_ptr<BpContents> c) {
    if (c == nullptr) return;
    if (c->IsLeaf()) {
        if (ImGui::Selectable(c->GetName().c_str(), _drag_item.lock().get() == c.get())) {
            // 显示详情
            LOG(INFO) << "select " << c->GetName().c_str();
        }
        // 初始状态&&在选中的item上鼠标按下，设置开始拖动
        if (_drag_state == 0 && ImGui::IsMouseDown(0) && ImGui::IsItemHovered()) {
            _drag_state = 1;
            _drag_item = c;
        }
        // 在开始拖动状态下，鼠标移动，设置拖动中状态
        ImVec2 mouse_delta = ImGui::GetMouseDragDelta(0);
        if (_drag_state == 1 && (mouse_delta.x != 0 || mouse_delta.y != 0)) {
            _drag_state = 2;
            // 给drag window发送展示消息
            Json::Value v;
            v["command"] = "show";
            v["desc"] = _drag_item.lock()->GetName();
            SendMessage({PanelName(), "drag tip", "", v});
        }
        // 鼠标松开，初始化状态
        if (_drag_state == 2 && ImGui::IsMouseReleased(0)) {
            // 给drag tip发送关闭展示消息
            Json::Value v;
            v["command"] = "close";
            SendMessage({PanelName(), "drag tip", "", v});
            // 给 bp editor发送创建Node消息
            Json::Value v2;
            v2["command"] = "spawn_node";
            v2["node_name"] = (_drag_item.lock()->GetType() == BpContents::Type::FUNC) ? _drag_item.lock()->GetFullPath() : _drag_item.lock()->GetName();
            v2["type"] = (int)_drag_item.lock()->GetType();
            ImVec2 xy = ImGui::GetMousePos();
            v2["x"] = xy.x;
            v2["y"] = xy.y;
            SendMessage({PanelName(), "editor", "", v2});
            _drag_state = 0;
            _drag_item.reset();
        }
        return;
    }
    if (ImGui::TreeNode(c->GetName().c_str())) {
        auto children = c->GetChildren();
        for (int i = 0; i < children.size(); ++i) {
            ShowLib(children[i]);
        }
        ImGui::TreePop();
    }
}

} // namespace sfe