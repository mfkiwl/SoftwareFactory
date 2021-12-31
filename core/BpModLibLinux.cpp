#include <sys/types.h>
#include <dirent.h>
#include "BpModuleLinux.hpp"
#include "BpModLibLinux.hpp"

namespace bp {

std::vector<std::string> BpModLibLinux::GetDirFiles(const std::string& conf_path) {
    std::vector<std::string> res;
    DIR* dir = opendir(conf_path.c_str());
    if (dir == nullptr) {
        return res;
    }
    struct dirent* entry = nullptr;
    while (nullptr != (entry = readdir(dir))) {
        if (entry->d_type == DT_REG) {
            res.emplace_back(conf_path + entry->d_name);
        }
    }
    closedir(dir);
    return res;
}

std::shared_ptr<BpModule> BpModLibLinux::CreateModule(const std::string& conf_file) {
    auto mod = std::make_shared<BpModuleLinux>();
    if (!mod->LoadModule(conf_file)) {
        return nullptr;
    }
    return mod;
}

} // namespace bp