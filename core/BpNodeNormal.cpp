#include "BpNodeNormal.hpp"
#include "BpGraph.hpp"
#include "Bp.hpp"

namespace bp {

BpNodeNormal::BpNodeNormal(const std::string& func_full_path, std::shared_ptr<BpGraph> parent)
	: BpNode(func_full_path, parent, BpObjType::BP_NODE_NORMAL)
{
    // 创建一个输入输出flow
    AddPin("", BpPinKind::BP_INPUT, BpPinType::BP_FLOW, BpVariable());
    AddPin("", BpPinKind::BP_OUTPUT, BpPinType::BP_FLOW, BpVariable());
    // 根据函数名获得函数指针
    _f = Bp::Instance().GetBaseLib()->GetFunc(func_full_path);
    // 根据函数输入参数设置输入pin
    for (int i = 0; i < _f.args.size(); ++i) {
        // TODO
        // 根据描述创建变量
        AddPin(_f.args[i], BpPinKind::BP_INPUT, BpPinType::BP_VALUE, BpVariable());
    }
    // 根据函数输出参数设置输出pin
    for (int i = 0; i < _f.res.size(); ++i) {
        AddPin(_f.res[i], BpPinKind::BP_OUTPUT, BpPinType::BP_VALUE, BpVariable());
    }
}

void BpNodeNormal::Logic() {
    // TODO
    // 根据输入参数,输出参数数量,转换指针并调用
    // 获得返回值并设置到输出pin保存的值里
}

} // namespace bp