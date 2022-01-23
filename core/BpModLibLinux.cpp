#include "BpModuleLinux.hpp"
#include "BpModLibLinux.hpp"

namespace bp {

std::shared_ptr<BpModule> BpModLibLinux::CreateModule(const std::string& conf_file) {
    auto mod = std::make_shared<BpModuleLinux>();
    if (!mod->LoadModule(conf_file)) {
        return nullptr;
    }
    return mod;
}

} // namespace bp