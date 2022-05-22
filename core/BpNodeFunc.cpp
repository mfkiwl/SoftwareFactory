#include "BpNodeFunc.hpp"
#include "BpGraph.hpp"
#include "Bp.hpp"

namespace bp {

BpNodeFunc::BpNodeFunc(const std::string& func_full_path, std::shared_ptr<BpGraph> parent)
	: BpNode(func_full_path, parent)
{
    _node_type = BpNodeType::BP_NODE_FUNC;
}

BpNodeRunState BpNodeFunc::Logic() {
    // 根据输入参数,输出参数数量,转换指针并调用
    // 获得返回值并设置到输出pin保存的值里
    int in_n = _info.type_args.size();
    int out_n = _info.type_res.size();
    std::vector<pb_msg_ptr_t> args;
    for (auto& in : _inputs) {
        if (in.GetPinType() == BpPinType::BP_VALUE) {
            args.emplace_back(in.Get<pb_msg_t>());
        }
    }
    int i = 0;
    if (_info.type == BpModuleFuncType::RES1_ARG0) {
        auto res = reinterpret_cast<module_func0_t>(_info.func)();
        for(auto& out : _outputs) {
            if (out.GetPinType() == BpPinType::BP_VALUE) {
                out.SetValue(res);
                break;
            }
        }
    } else if (_info.type == BpModuleFuncType::RESN_ARG0) {
        auto res = reinterpret_cast<module_func1_t>(_info.func)();
        for(auto& out : _outputs) {
            if (out.GetPinType() == BpPinType::BP_VALUE) {
                out.SetValue(res[i++]);
            }
        }
    } else if (_info.type == BpModuleFuncType::RES1_ARG1) {
        auto res = reinterpret_cast<module_func2_t>(_info.func)(args[0]);
        for(auto& out : _outputs) {
            if (out.GetPinType() == BpPinType::BP_VALUE) {
                out.SetValue(res);
                break;
            }
        }
    } else if (_info.type == BpModuleFuncType::RESN_ARG1) {
        auto res = reinterpret_cast<module_func3_t>(_info.func)(args[0]);
        for(auto& out : _outputs) {
            if (out.GetPinType() == BpPinType::BP_VALUE) {
                out.SetValue(res[i++]);
            }
        }
    } else if (_info.type == BpModuleFuncType::RES1_ARGN) {
        auto res = reinterpret_cast<module_func4_t>(_info.func)(args);
        for(auto& out : _outputs) {
            if (out.GetPinType() == BpPinType::BP_VALUE) {
                out.SetValue(res);
                break;
            }
        }
    } else if (_info.type == BpModuleFuncType::RESN_ARGN) {
        auto res = reinterpret_cast<module_func5_t>(_info.func)(args);
        for(auto& out : _outputs) {
            if (out.GetPinType() == BpPinType::BP_VALUE) {
                out.SetValue(res[i++]);
            }
        }
    } else if (_info.type == BpModuleFuncType::RES0_ARG1) {
        reinterpret_cast<module_func6_t>(_info.func)(args[0]);
    } else if (_info.type == BpModuleFuncType::RES0_ARGN) {
        reinterpret_cast<module_func7_t>(_info.func)(args);
    }
    return BpNodeRunState::BP_RUN_LOGIC_OK;
}

} // namespace bp