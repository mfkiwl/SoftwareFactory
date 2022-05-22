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
        , _remain_cnt(loop)
    {
        _node_type = bp::BpNodeType::BP_NODE_EV;
    }
    
    /* 数字越小优先级越高 */
    int GetWeight() { return _lv; }
    int GetLoopCnt() { return _loop_cnt; }
    int LoopOnce() { return (_remain_cnt == -1) ? INT_MAX : (--_remain_cnt); }
    void ResetLoop() { _remain_cnt = _loop_cnt; }
protected:
    int _lv = 1;
    int _loop_cnt = 1;
    int _remain_cnt = 1;
};

struct BpNodeEvCmp {
    bool operator()(const std::shared_ptr<BpNodeEv>& a, const std::shared_ptr<BpNodeEv>& b) const {
        return a->GetWeight() < b->GetWeight();
    }
};

} // namespace bp