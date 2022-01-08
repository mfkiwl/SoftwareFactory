#include "BpNode.hpp"
#include "BpGraph.hpp"

namespace bp {

BpNode::BpNode(const std::string& name, std::shared_ptr<BpGraph> parent, BpObjType t)
	: BpObj(name, parent)
{
	_obj_type = t;
}

} // namespace bp