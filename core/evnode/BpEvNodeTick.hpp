#pragma once
#include <vector>

#include "BpEvNode.hpp"

namespace bp {

/* 常规的函数节点 */
class BpEvNodeTick : public BpEvNode
{
public:
	BpEvNodeTick(std::shared_ptr<BpGraph> parent);

	virtual void Logic() override;
};

} // namespace bp