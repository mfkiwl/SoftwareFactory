#include "SFEPanelUINodes.hpp"
#include "Bp.hpp"
#include "ui_node/SFEUINodePrint.hpp"

namespace sfe {

bool SFEPanelUINodes::Init() {
    SetPanelName("uinodes");
    // 创建UINode目录
    _uinode_contents = std::make_shared<BpContents>(nullptr, "ui", ::BpContents::Type::CONTENTS);
    _uinode_contents->AddChild(std::make_shared<BpContents>(nullptr, "Print", ::BpContents::Type::LEAF, ::BpContents::LeafType::USER));
    
    bp::Bp::Instance().RegisterUserMod(_uinode_contents, std::bind(&SFEPanelUINodes::SpawnUINode, this, std::placeholders::_1));
    return true;
}

void SFEPanelUINodes::Update() {
}

void SFEPanelUINodes::Exit() {

}

std::shared_ptr<bp::BpNode> SFEPanelUINodes::SpawnUINode(const std::string& full_path) {
    if (full_path == "ui.Print") {
        auto node = std::make_shared<SFEUINodePrint>(full_path, nullptr, shared_from_this()); 
        node->AddPin("", ::bp::BpPinKind::BP_INPUT, ::bp::BpPinType::BP_FLOW, ::bp::BpVariable()); 
        node->AddPin("any", ::bp::BpPinKind::BP_INPUT, ::bp::BpPinType::BP_VALUE, ::bp::BpVariable("any", "any", nullptr)).SetAssignByRef(true); 
        return node;
    }
    return nullptr;
}

} // namespace sfe