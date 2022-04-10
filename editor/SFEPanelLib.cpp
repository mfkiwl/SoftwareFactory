#include "SFEPanelLib.hpp"
#include "Bp.hpp"
#include "bpcommon.hpp"

namespace sfe {

bool SFEPanelLib::Init() {
    return true;
}

void SFEPanelLib::Update() {
    ImGui::Begin(PanelName().c_str());
    auto root = bp::Bp::Instance().GetContents();
    auto children = root->GetChildren();
    for (int i = 0; i < children.size(); ++i) {
        ShowLib(children[i]);
    }
    if (_show_var_setting) {
        _show_var_setting = false;
        ImGui::OpenPopup("variable setting");
    }
    ShowVarSetting();
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
            SendMessage("drag tip", v);
        }
        // 鼠标松开，初始化状态
        if (_drag_state == 2 && ImGui::IsMouseReleased(0)) {
            // 给drag tip发送关闭展示消息
            Json::Value v;
            v["command"] = "close";
            SendMessage("drag tip", v);
            // 给 bp editor发送创建Node消息
            // 变量节点, 需要使用界面设置get/set和变量名称
            if (_drag_item.lock()->GetLeafType() == BpContents::LeafType::VAL) {
                // show setting
                _drag_info.clear();
                ImVec2 xy = ImGui::GetMousePos();
                _drag_info["x"] = xy.x;
                _drag_info["y"] = xy.y;
                _show_var_setting = true;
            } else {
                Json::Value v2;
                v2["command"] = "spawn_node";
                auto leaf_type = _drag_item.lock()->GetLeafType();
                v2["node_name"] = (leaf_type == BpContents::LeafType::FUNC || leaf_type == BpContents::LeafType::USER) ? _drag_item.lock()->GetFullPath() : _drag_item.lock()->GetName();
                v2["type"] = (int)_drag_item.lock()->GetLeafType();
                ImVec2 xy = ImGui::GetMousePos();
                v2["x"] = xy.x;
                v2["y"] = xy.y;
                SendMessage("editor", v2);
                _drag_item.reset();
            }
            _drag_state = 0;
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

void SFEPanelLib::ShowVarSetting() {
    if (ImGui::BeginPopupModal("variable setting", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static bool is_get = true;
        static int style_idx = 0;
        if (ImGui::Combo("get/set", &style_idx, "get\0set\0")) {
            switch (style_idx) {
            case 0: is_get = true; break;
            case 1: is_get = false; break;
            }
        }
        static char buf[64] = "";
        ImGui::InputText("variable name", buf, 64, ImGuiInputTextFlags_CharsNoBlank);
        if (ImGui::Button("OK", ImVec2(120, 0))) { 
            if (!bp::BpCommon::IsName(buf, strlen(buf))) {
                LOG(WARNING) << "name is not availdable";
            } else {
                Json::Value v2;
                v2["command"] = "spawn_node";
                v2["node_name"] = _drag_item.lock()->GetName();
                v2["type"] = (int)_drag_item.lock()->GetLeafType();
                v2["is_get"] = is_get;
                v2["var_name"] = buf;
                v2["x"] = _drag_info["x"];
                v2["y"] = _drag_info["y"];
                SendMessage("editor", v2);
            }
            _drag_item.reset();
            ImGui::CloseCurrentPopup();
            memset(buf, 0, sizeof(buf));
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { 
            _drag_item.reset();
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
    }
}

} // namespace sfe