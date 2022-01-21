#pragma once

#include <vector>

#include "BpNode.hpp"
#include "BpModule.hpp"

namespace bp {

/* 常规的函数节点 */
class BpNodeFunc : public BpNode
{
public:
	BpNodeFunc(const std::string& func_full_path, std::shared_ptr<BpGraph> parent);

	virtual void Logic() override;

	void SetFuncInfo(const BpModuleFunc& info) { _info = info; }

private:
	BpModuleFunc _info;
};

} // namespace bp