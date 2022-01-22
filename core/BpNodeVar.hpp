#pragma once
#include "BpNode.hpp"

namespace bp {

class BpNodeVar : public BpNode
{
public:
	BpNodeVar(bool is_get, BpVariable& var, std::shared_ptr<BpGraph> parent)
        : BpNode(var.GetName(), parent)
        , _is_get(is_get)
        , _var(var)
    {
        _node_type = BpNodeType::BP_NODE_VAR;
        _node_style = BpNodeStyle::BP_SIMPLE;
    }

	virtual void Logic() {}

    bool IsGet() { return _is_get; }
private:
    bool _is_get = true;
    BpVariable _var;
};

} // namespace bp