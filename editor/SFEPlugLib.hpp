#pragma once
#include <unordered_map>
#include <unordered_set>

#include "bpflags.hpp"
#include "bpcommon.hpp"
#include "SFEPanel.hpp"
#include "SFEPlug.hpp"

namespace sfe {
class SFEPlugLib {
public:
    bool Init(const std::string& plug_conf_path) {
        auto confs = bp::BpCommon::GetDirFiles(plug_conf_path);
        if (confs.empty()) {
            LOG(ERROR) << "search plug conf file " << plug_conf_path << " failed";
            return false;
        }
        for (auto & conf : confs) {
            auto json_val = bp::BpCommon::LoadJsonFromFile(conf);
            if (json_val.isNull()) {
                LOG(WARNING) << "load plug conf file " << conf << " failed";
                continue;
            }
            if (false == LoadPlug(json_val)) {
                LOG(ERROR) << "load plug " << conf << " failed";
                continue;
            }
        }
        return true;
    }

private:
    bool LoadPlug(const Json::Value& root) {
        // 提取模块名称
        auto mod_name = root["_lib"].asString();
        mod_name = mod_name.substr(3, mod_name.size() - 6);
        // 初始化dll
        auto plug = std::make_shared<sfe::SFEPlug>();
        std::string dll_path = bp::FLAGS_base_mod_lib_dir + root["_lib"].asString();
        if (!plug->Init(dll_path.c_str())) {
            LOG(ERROR) << "load plug " << dll_path << " failed";
            return false;
        }
        if (_plugs.find(mod_name) != _plugs.end()) {
            LOG(ERROR) << "plug " << dll_path << " is already loaded";
            return false;
        }
        _plugs[mod_name] = plug;
        ////
        int32_t count = root["_instance"].size();
        for (int32_t j = 0; j < count; ++j) {
            auto inst = root["_instance"][j];
            // 创建panel实例
            auto panel = plug->GetCreateFunc()(inst["type"].asString());
            // 注册panel
            if (!SFEPanel::RegPanel(inst["name"].asString(), panel)) {
                LOG(ERROR) << "plug " << dll_path << " register failed";
                return false;
            }
            if (_plug_panel_names.find(mod_name) == _plug_panel_names.end()) {
                _plug_panel_names[mod_name] = std::unordered_set<std::string>();
            }
            if (_plug_panel_names[mod_name].find(inst["type"].asString()) != _plug_panel_names[mod_name].end()) {
                continue;
            }
            _plug_panel_names[mod_name].insert(inst["type"].asString());
        }
        return true;
    }

    /* key: libname, value: create_panel function pointer */
    std::unordered_map<std::string, std::shared_ptr<SFEPlug>> _plugs;
    /* key: libname, value: panel names */
    std::unordered_map<std::string, std::unordered_set<std::string>> _plug_panel_names;
};

} // namespace sfe