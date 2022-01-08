#pragma once
#include "BpModule.hpp"

namespace bp {
class BpNode;
class BpGraph;
class BpNodeLib
{
    typedef std::shared_ptr<BpNode> create_func_node_t(BpModuleFunc func_info);
    typedef std::shared_ptr<BpNode> create_var_node_t(pb_msg_ptr_t msg, bool is_get, bool is_ref);
public:
    /* 创建函数node */
    std::shared_ptr<BpNode> CreateFuncNode(BpModuleFunc func_info);

    /* 创建变量node */
    std::shared_ptr<BpNode> CreateVarNode(pb_msg_ptr_t msg, bool is_get, bool is_ref);
    
    /* 创建事件Node */
    std::shared_ptr<BpNode> CreateEvNode(const std::string& name);

    /* 创建分支结构Node */
    std::shared_ptr<BpNode> CreateBaseNode(const std::string& name);

    /* 创建BpGraph的IO Node */
    std::shared_ptr<BpNode> CreateGraphIONode(const std::shared_ptr<BpGraph>& g);
};

} // namespace bp