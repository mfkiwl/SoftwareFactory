#pragma once
#include <functional>
#include <unordered_map>
#include "BpModule.hpp"
#include "BpVariable.hpp"

namespace bp {
class BpNode;
class BpNodeEv;
class BpGraph;
class BpNodeLib
{
    typedef std::shared_ptr<BpNodeEv> create_ev_node_t();
    typedef std::shared_ptr<BpNode> create_base_node_t();
public:
    BpNodeLib();
    virtual ~BpNodeLib();

    /* 创建函数Node */
    std::shared_ptr<BpNode> CreateFuncNode(BpModuleFunc func_info, 
            std::vector<BpVariable>& args,
            std::vector<BpVariable>& res);

    /* 创建变量Node */
    std::shared_ptr<BpNode> CreateVarNode(const std::string& var_name, BpVariable var, bool is_get);
    
    /* 创建事件Node */
    std::shared_ptr<BpNode> CreateEvNode(const std::string& name);

    /* 创建分支结构Node */
    std::shared_ptr<BpNode> CreateBaseNode(const std::string& name);

    /* 获得节点(分支结构)目录 */
    std::shared_ptr<BpContents> GetContents();

private:
    std::shared_ptr<BpContents> _root_contents;
    std::shared_ptr<BpContents> _ev_contents;
    std::shared_ptr<BpContents> _base_contents;
    std::unordered_map<std::string, std::function<create_ev_node_t>> _ev_nodes;
    std::unordered_map<std::string, std::function<create_base_node_t>> _base_nodes;
};

} // namespace bp