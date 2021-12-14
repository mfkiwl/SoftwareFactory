#ifndef __BP_LIB_HPP__
#define __BP_LIB_HPP__
#include <memory>
#include <string>
#include <vector>

#include "BpModule.hpp"

namespace bp {
/*
 搜索配置文件，并根据配置文件加载模块
 */
class BpLib
{
public:
    BpLib() = default;

    bool Init(const std::string& conf_path) {
        auto confs = GetDirFiles(conf_path);
        if (confs.empty()) {
            std::cerr << "search conf file " << conf_path << " failed" << std::endl;
            return false;
        }
        for (auto & conf : confs) {
            auto mod = CreateModule(conf);
            if (mod == nullptr) {
                std::cerr << "create module " << conf << " failed" << std::endl;
                continue;
            }
            _mods.emplace_back(mod);
        }
        return true;
    }

    std::vector<std::shared_ptr<BpModule>>& GetMods() {
        return _mods;
    }
protected:
    virtual std::vector<std::string> GetDirFiles(const std::string& conf_path) = 0;
    virtual std::shared_ptr<BpModule> CreateModule(const std::string& mod_name) = 0;

    std::vector<std::shared_ptr<BpModule>> _mods;
};

} // namespace bp

#endif