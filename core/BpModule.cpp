#include "BpModule.hpp"

namespace bp {

/*
    生成contents树目录
    生成_module_funcs
    生成_create_var_funcs
*/
bool BpModule::LoadModule(const std::string& json_path) {
    return false;
}

std::shared_ptr<BpContents> BpModule::GetContents() {
    return _contents;
}

BpModule::pb_msg_t BpModule::CreateModuleVal(const std::string& msg_name) {
    if (_create_var_funcs.find(msg_name) == _create_var_funcs.end()) {
        return nullptr;
    }
    return nullptr;
}

BpModuleFunc BpModule::GetModuleFunc(const std::string& func_name) {
    if (_module_funcs.find(func_name) == _module_funcs.end()) {
        return BpModuleFunc();
    }
    return BpModuleFunc();
}

} // namespace bp