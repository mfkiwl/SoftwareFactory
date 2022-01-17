#include "BpNodeNormal.hpp"
#include "BpGraph.hpp"
#include "Bp.hpp"

namespace bp {

BpNodeNormal::BpNodeNormal(const std::string& func_full_path, std::shared_ptr<BpGraph> parent)
	: BpNode(func_full_path, parent, BpObjType::BP_NODE_NORMAL)
{}

void BpNodeNormal::Logic() {
    // 根据输入参数,输出参数数量,转换指针并调用
    // 获得返回值并设置到输出pin保存的值里
    // TODO
    // int in_n = _info.type_args.size();
    // int out_n = _info.type_res.size();
    // std::vector<pb_msg_ptr_t> args;
    // std::vector<pb_msg_ptr_t> res;
    // for (auto& in : _inputs) {
    //     if (in.GetPinType() == BpPinType::BP_VALUE) {
    //         args.emplace_back(in.GetValue());
    //     }
    // }
    // for(auto& out : _outputs) {
    //     if (out.GetPinType() == BpPinType::BP_VALUE) {
    //         res.emplace_back(out.GetValue());
    //     }
    // }
    // if (_info.type == BpModuleFuncType::RES1_ARG0) {
    //     res[0] = std::any_cast<module_func0_t>(_info.func)();
    // } else if (_info.type == BpModuleFuncType::RESN_ARG0) {
    //     auto r = std::any_cast<module_func0_t>(_info.func)();
    // } else if (_info.type == BpModuleFuncType::RES1_ARG1) {
        
    // } else if (_info.type == BpModuleFuncType::RESN_ARG1) {
        
    // } else if (_info.type == BpModuleFuncType::RES1_ARGN) {
        
    // } else if (_info.type == BpModuleFuncType::RESN_ARGN) {
        
    // }
    LOG(INFO) << "run " << _name;
}

} // namespace bp