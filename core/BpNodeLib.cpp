#include "BpNodeLib.hpp"
#include "BpNodeVar.hpp"
#include "BpNodeFunc.hpp"
#include "BpNodeEvTick.hpp"
#include "basenode/BpNodeBaseDelay.hpp"
#include "basenode/BpNodeBaseBranch.hpp"

namespace bp {
BpNodeLib::BpNodeLib() 
    : _ev_nodes {
        {"Tick", [](){ 
            auto node = std::make_shared<BpNodeEvTick>(nullptr); 
            node->AddPin("", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable()); 
            return node; }
        }
    },
    _base_nodes {
        {"Delay", [](){
            auto node = std::make_shared<BpNodeBaseDelay>(nullptr, 0.0f); 
            node->Init();
            return node;
        }},
        {"Branch", [](){
            auto node = std::make_shared<BpNodeBaseBranch>(nullptr);
            node->Init();
            return node;
        }}
    }
{
    _root_contents = std::make_shared<BpContents>(nullptr, "", BpContents::Type::CONTENTS);
    // 创建事件目录
    _ev_contents = std::make_shared<BpContents>(nullptr, "event", BpContents::Type::CONTENTS);
    _ev_contents->AddChild(std::make_shared<BpContents>(nullptr, "Tick", BpContents::Type::LEAF, BpContents::LeafType::EV));
    _root_contents->AddChild(_ev_contents);
    // 创建基础节点目录
    _base_contents = std::make_shared<BpContents>(nullptr, "base", BpContents::Type::CONTENTS);
    _base_contents->AddChild(std::make_shared<BpContents>(nullptr, "Delay", BpContents::Type::LEAF, BpContents::LeafType::BASE));
    _base_contents->AddChild(std::make_shared<BpContents>(nullptr, "Branch", BpContents::Type::LEAF, BpContents::LeafType::BASE));
    _root_contents->AddChild(_base_contents);
}

BpNodeLib::~BpNodeLib() {
}

std::shared_ptr<BpNode> BpNodeLib::CreateFuncNode(BpModuleFunc func_info, 
            std::vector<BpVariable>& args,
            std::vector<BpVariable>& res) {
    LOG(INFO) << "create func node " << func_info.name;
    auto node = std::make_shared<BpNodeFunc>(func_info.name, nullptr);
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
    return node;
}

/* 创建变量Node */
std::shared_ptr<BpNode> BpNodeLib::CreateVarNode(BpVariable var, bool is_get) {
    LOG(INFO) << "create var node " << var.GetName() << "(" << var.GetType() << ")";
    auto node = std::make_shared<BpNodeVar>(is_get, var, nullptr);
    if (is_get) {
        node->AddPin(var.GetType(), BpPinKind::BP_OUTPUT, BpPinType::BP_VALUE, var);
    } else {
        node->AddPin("", BpPinKind::BP_INPUT, BpPinType::BP_FLOW, BpVariable());
        node->AddPin("", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable());
        node->AddPin(var.GetType(), BpPinKind::BP_INPUT, BpPinType::BP_VALUE, var);
    }
    return node;
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
std::shared_ptr<BpNode> BpNodeLib::CreateBaseNode(const std::string& name) {
    if (_base_nodes.find(name) == _base_nodes.end()) {
        LOG(ERROR) << "can't find base node " << name;
        return nullptr;
    }
    return _base_nodes[name]();
}

std::shared_ptr<BpContents> BpNodeLib::GetContents() {
    return _root_contents;
}

} // namespace bp