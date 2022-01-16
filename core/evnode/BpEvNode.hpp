#pragma once
#include "../BpNode.hpp"

namespace bp {

class BpEvNode : public BpNode
{
public:
	BpEvNode(const std::string& name, std::shared_ptr<BpGraph> parent, int lv, int loop)
        : BpNode(name, parent, bp::BpObjType::BP_NODE_EV)
        , _lv(lv)
        , _loop_cnt(loop) {}

	virtual void Logic() {};

    int GetWeight() { return _lv; }
    int GetLoopCnt() { return _loop_cnt; }
    
protected:
    int _lv = 1;
    int _loop_cnt = 1;
};

} // namespace bp