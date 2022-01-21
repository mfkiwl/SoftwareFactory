#ifndef __BpNode_hpp__
#define __BpNode_hpp__
#include <vector>
#include "BpObj.hpp"

namespace bp {

class BpNode : public BpObj
{
public:
	BpNode(const std::string& name, std::shared_ptr<BpGraph> parent, BpNodeType t = BpNodeType::BP_NODE_FUNC);

	virtual void Logic() {};
};

} // namespace bp
#endif