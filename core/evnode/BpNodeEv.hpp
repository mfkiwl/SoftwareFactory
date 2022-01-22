#pragma once
#include "BpNode.hpp"

namespace bp {

class BpNodeEv : public BpNode
{
public:
	BpNodeEv(const std::string& name, std::shared_ptr<BpGraph> parent, int lv, int loop)
        : BpNode(name, parent)
        , _lv(lv)
        , _loop_cnt(loop) 
    {
        _node_type = bp::BpNodeType::BP_NODE_EV;
    }

	virtual void Logic() {};

    int GetWeight() { return _lv; }
    int GetLoopCnt() { return _loop_cnt; }
    
protected:
    int _lv = 1;
    int _loop_cnt = 1;
};

} // namespace bp