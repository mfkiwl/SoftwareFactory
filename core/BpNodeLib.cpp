#include "BpNodeLib.hpp"
#include "BpNode.hpp"
#include "BpNodeNormal.hpp"
#include "BpEvNodeTick.hpp"

namespace bp {
BpNodeLib::BpNodeLib() 
    : _ev_nodes{
        {"Tick", [](){ 
            auto node = std::make_shared<BpEvNodeTick>(nullptr); 
            node->AddPin("", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable()); 
            return node; }
        }
    }
{
    _root_contents = std::make_shared<BpContents>(nullptr, BpContents::Type::CONTENTS, "");
    // 创建事件目录
    _ev_contents = std::make_shared<BpContents>(nullptr, BpContents::Type::CONTENTS, "event");
    _ev_contents->AddChild(std::make_shared<BpContents>(nullptr, BpContents::Type::EV, "Tick"));

    _root_contents->AddChild(_ev_contents);
}
BpNodeLib::~BpNodeLib() {

}

std::shared_ptr<BpNode> BpNodeLib::CreateFuncNode(BpModuleFunc func_info, 
            std::vector<BpVariable>& args,
            std::vector<BpVariable>& res) {
    LOG(INFO) << "create node " << func_info.name;
    auto node = std::make_shared<BpNodeNormal>(func_info.name, nullptr);
    node->SetFuncInfo(func_info);
    // // 创建一个输入输出flow
    node->AddPin("", BpPinKind::BP_INPUT, BpPinType::BP_FLOW, BpVariable());
    node->AddPin("", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable());
    // 根据函数输入参数设置输入pin
    for (int i = 0; i < func_info.type_args.size(); ++i) {
        // 根据描述创建变量
        node->AddPin(func_info.type_args[i], BpPinKind::BP_INPUT, BpPinType::BP_VALUE, args[i]);
    }
    // 根据函数输出参数设置输出pin
    for (int i = 0; i < func_info.type_res.size(); ++i) {
        node->AddPin(func_info.type_res[i], BpPinKind::BP_OUTPUT, BpPinType::BP_VALUE, res[i]);
    }
    LOG(INFO) << "create node end";
    return node;
}

std::shared_ptr<BpNode> BpNodeLib::CreateVarNode(pb_msg_ptr_t msg, bool is_get, bool is_ref) {
    return nullptr;
}

/* 创建事件Node */
std::shared_ptr<BpNode> BpNodeLib::CreateEvNode(const std::string& name) {
    if (_ev_nodes.find(name) == _ev_nodes.end()) {
        LOG(ERROR) << "can't find ev node " << name;
        return nullptr;
    }
    return _ev_nodes[name]();
}

/* 创建分支结构Node */
// TODO
std::shared_ptr<BpNode> BpNodeLib::CreateBaseNode(const std::string& name) {
    return nullptr;
}

std::shared_ptr<BpNode> BpNodeLib::CreateGraphIONode(const std::shared_ptr<BpGraph>& g) {
    return nullptr;
}

std::shared_ptr<BpContents> BpNodeLib::GetContents() {
    return _root_contents;
}

} // namespace bp