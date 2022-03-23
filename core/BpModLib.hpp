#ifndef __BP_LIB_HPP__
#define __BP_LIB_HPP__
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <glog/logging.h>
#include "bpcommon.hpp"
#include "BpModule.hpp"

namespace bp {
/*
 搜索配置文件，并根据配置文件加载模块
 */
class BpModLib
{
public:
    BpModLib() = default;
    ~BpModLib() {
        _mods.clear();
    }
    bool Init(const std::string& conf_path) {
        auto confs = BpCommon::GetDirFiles(conf_path);
        if (confs.empty()) {
            LOG(ERROR) << "search conf file " << conf_path << " failed";
            return false;
        }
        for (auto & conf : confs) {
            auto mod = CreateModule(conf);
            if (mod == nullptr) {
                LOG(ERROR) << "create module " << conf << " failed";
                continue;
            }
            _mods.emplace_back(mod);
        }
        return true;
    }

    std::vector<std::shared_ptr<BpModule>>& GetMods() {
        return _mods;
    }

    BpModuleVar CreateVal(const std::string& msg_name) {
        // 搜索到对应模块,调用对应模块创建变量对象
        auto mod_name = BpCommon::GetModName(msg_name);
        for (int i = 0; i < _mods.size(); ++i) {
            if (_mods[i]->Name() == mod_name) {
                BpModuleVar bpvar;
                bpvar.var = _mods[i]->CreateModuleVal(msg_name);
                bpvar.desc = _mods[i]->GetModuleVarDesc(msg_name);
                return bpvar;
            }
        }
        LOG(ERROR) << "Can't find var type \"" << msg_name << "\"";
        return BpModuleVar();
    }

    BpModuleFunc GetFunc(const std::string& full_path) {
        // 解析全路径,搜索到对应模块,获得对应模块函数指针
        auto mod_name = BpCommon::GetModName(full_path);
        auto pos = full_path.rfind('.');
        auto func_name = full_path.substr(pos + 1);
        for (int i = 0; i < _mods.size(); ++i) {
            if (_mods[i]->Name() == mod_name) {
                return _mods[i]->GetModuleFunc(func_name);
            }
        }
        LOG(ERROR) << "Can't find \"" << func_name << "\" in " << full_path;
        return BpModuleFunc();
    }

protected:
    virtual std::shared_ptr<BpModule> CreateModule(const std::string& mod_name) = 0;

    std::vector<std::shared_ptr<BpModule>> _mods;
};

} // namespace bp

#endif