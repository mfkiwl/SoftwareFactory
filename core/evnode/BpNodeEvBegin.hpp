#pragma once
#include <vector>

#include "BpNodeEv.hpp"

namespace bp {

/* 常规的函数节点 */
class BpNodeEvBegin : public BpNodeEv
{
public:
	BpNodeEvBegin(std::shared_ptr<BpGraph> parent)
	    : BpNodeEv("Begin", parent, 99, 1)
    {}

	void Logic() override {}
};

} // namespace bp