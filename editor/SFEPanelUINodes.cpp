#include "SFEPanelUINodes.hpp"
#include "Bp.hpp"
#include "ui_node/SFEUINodePlot.hpp"

namespace sfe {

bool SFEPanelUINodes::Init() {
    // 创建UINode目录
    _uinode_contents = std::make_shared<BpContents>(nullptr, "ui", ::BpContents::Type::CONTENTS);
    _uinode_contents->AddChild(std::make_shared<BpContents>(nullptr, "Plot", ::BpContents::Type::LEAF, ::BpContents::LeafType::USER));
    
    bp::Bp::Instance().RegisterUserMod(_uinode_contents, std::bind(&SFEPanelUINodes::SpawnUINode, this, std::placeholders::_1));
    return true;
}

void SFEPanelUINodes::Update() {
}

void SFEPanelUINodes::Exit() {

}

std::shared_ptr<bp::BpNode> SFEPanelUINodes::SpawnUINode(const std::string& full_path) {
    if (full_path == "ui.Plot") {
        auto node = std::make_shared<SFEUINodePlot>(full_path, nullptr, shared_from_this()); 
        node->AddPin("", ::bp::BpPinKind::BP_INPUT, ::bp::BpPinType::BP_FLOW, ::bp::BpVariable()); 
        node->AddPin("", ::bp::BpPinKind::BP_OUTPUT, ::bp::BpPinType::BP_FLOW, ::bp::BpVariable()); 
        std::string cmd_type = "bpbase.String";
        auto cmd = bp::Bp::Instance().CreateVariable(cmd_type, "cmd");
        node->AddPin(cmd_type, ::bp::BpPinKind::BP_INPUT, ::bp::BpPinType::BP_VALUE, cmd); 
        std::string var_type = "bpbase.Float";
        auto x = bp::Bp::Instance().CreateVariable(var_type, "x");
        auto y = bp::Bp::Instance().CreateVariable(var_type, "y");
        node->AddPin(var_type, ::bp::BpPinKind::BP_INPUT, ::bp::BpPinType::BP_VALUE, x); 
        node->AddPin(var_type, ::bp::BpPinKind::BP_INPUT, ::bp::BpPinType::BP_VALUE, y); 
        return node;
    }
    return nullptr;
}

} // namespace sfe