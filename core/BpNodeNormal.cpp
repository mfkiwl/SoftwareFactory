#include "BpNodeNormal.hpp"
#include "BpGraph.hpp"
#include "Bp.hpp"

namespace bp {

BpNodeNormal::BpNodeNormal(const std::string& func_full_path, std::shared_ptr<BpGraph> parent)
	: BpNode(func_full_path, parent, BpObjType::BP_NODE_NORMAL)
{}

void BpNodeNormal::Logic() {
    // TODO
    // 根据输入参数,输出参数数量,转换指针并调用
    // 获得返回值并设置到输出pin保存的值里
}

} // namespace bp