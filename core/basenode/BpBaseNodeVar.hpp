#pragma once
#include "../BpNode.hpp"

namespace bp {

class BpBaseNodeVar : public BpNode
{
public:
	BpBaseNodeVar(bool is_get, BpVariable& var, std::shared_ptr<BpGraph> parent)
        : BpNode(var.GetName(), parent, bp::BpNodeType::BP_NODE_VAR)
        , _is_get(is_get)
        , _var(var)
    {}

	virtual void Logic() {}

    bool IsGet() { return _is_get; }
private:
    bool _is_get = true;
    BpVariable _var;
};

} // namespace bp