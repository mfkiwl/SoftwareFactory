#pragma once

#include <vector>

#include "BpNode.hpp"
#include "BpModule.hpp"

namespace bp {

/* 常规的函数节点 */
class BpNodeNormal : public BpNode
{
public:
	BpNodeNormal(const std::string& func_full_path, std::shared_ptr<BpGraph> parent);

	virtual void Logic() override;
};

} // namespace bp