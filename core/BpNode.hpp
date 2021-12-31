#ifndef __BpNode_hpp__
#define __BpNode_hpp__
#include <vector>
#include "BpObj.hpp"

namespace bp {

class BpNode : public BpObj
{
public:
	BpNode(const std::string& name, std::shared_ptr<BpGraph> parent, BpObjType t = BpObjType::BP_NODE_NORMAL);

	virtual void Logic() = 0;
};

} // namespace bp
#endif