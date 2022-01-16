#pragma once
#include <functional>
#include <unordered_map>
#include "BpModule.hpp"
#include "BpVariable.hpp"

namespace bp {
class BpNode;
class BpEvNode;
class BpGraph;
class BpNodeLib
{
    typedef std::shared_ptr<BpEvNode> create_ev_node_t();
    typedef std::shared_ptr<BpNode> create_func_node_t(BpModuleFunc func_info);
    typedef std::shared_ptr<BpNode> create_var_node_t(pb_msg_ptr_t msg, bool is_get, bool is_ref);
public:
    BpNodeLib();
    virtual ~BpNodeLib();

    /* 创建函数node */
    std::shared_ptr<BpNode> CreateFuncNode(BpModuleFunc func_info, 
            std::vector<BpVariable>& args,
            std::vector<BpVariable>& res);

    /* 创建变量node */
    std::shared_ptr<BpNode> CreateVarNode(pb_msg_ptr_t msg, bool is_get, bool is_ref);
    
    /* 创建事件Node */
    std::shared_ptr<BpNode> CreateEvNode(const std::string& name);

    /* 创建分支结构Node */
    std::shared_ptr<BpNode> CreateBaseNode(const std::string& name);

    /* 创建BpGraph的IO Node */
    std::shared_ptr<BpNode> CreateGraphIONode(const std::shared_ptr<BpGraph>& g);

    /* 获得节点(分支结构)目录 */
    std::shared_ptr<BpContents> GetContents();

private:
    std::shared_ptr<BpContents> _root_contents;
    std::shared_ptr<BpContents> _ev_contents;
    std::unordered_map<std::string, std::function<create_ev_node_t>> _ev_nodes;
};

} // namespace bp