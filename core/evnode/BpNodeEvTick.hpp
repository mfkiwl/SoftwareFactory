#pragma once
#include <vector>

#include "BpNodeEv.hpp"

namespace bp {

/* 常规的函数节点 */
class BpNodeEvTick : public BpNodeEv
{
public:
	BpNodeEvTick(std::shared_ptr<BpGraph> parent)
		: BpNodeEv("Tick", parent, 2, -1)
	{}

	void Logic() override {}
};

} // namespace bp