#include "BpNodeLib.hpp"
#include "BpNode.hpp"

namespace bp {

std::shared_ptr<BpNode> BpNodeLib::CreateFuncNode(BpModuleFunc func_info) {
    return nullptr;
}

std::shared_ptr<BpNode> BpNodeLib::CreateVarNode(pb_msg_ptr_t msg, bool is_get, bool is_ref) {
    return nullptr;
}

/* 创建事件Node */
// TODO
std::shared_ptr<BpNode> BpNodeLib::CreateEvNode(const std::string& name) {
    return nullptr;
}

/* 创建分支结构Node */
// TODO
std::shared_ptr<BpNode> BpNodeLib::CreateBaseNode(const std::string& name) {
    return nullptr;
}

std::shared_ptr<BpNode> BpNodeLib::CreateGraphIONode(const std::shared_ptr<BpGraph>& g) {
    return nullptr;
}

} // namespace bp