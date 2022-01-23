#pragma once
#include <memory>
#include <string>
#include <fstream>
#include <unordered_map>
#include <glog/logging.h>
#include "bpcommon.hpp"
#include "BpContents.hpp"

namespace bp {

/* 搜索组织模块配置文件，并根据配置文件加载组织模块 */
class BpGraphModLib
{
public:
    BpGraphModLib() = default;
    ~BpGraphModLib() {
        _mods.clear();
    }
    bool Init(const std::string& conf_path) {
        auto confs = BpCommon::GetDirFiles(conf_path);
        if (confs.empty()) {
            LOG(ERROR) << "search mod graph conf file " << conf_path << " failed";
            return false;
        }
        _contents = std::make_shared<BpContents>(nullptr, "graph", BpContents::Type::CONTENTS);
        for (auto & conf : confs) {
            auto mods = LoadGraphMod(conf);
            if (mods.size() <= 0) {
                LOG(ERROR) << "load mod graph " << conf << " failed";
                continue;
            }
            for (int i = 0; i < mods.size(); ++i) {
                auto mod_name = mods[i]["name"].asString();
                if (_mods.find(mod_name) != _mods.end()) {
                    LOG(WARNING) << "load mod graph " << mod_name << " exist, continue";
                    continue;
                }
                _mods[mod_name] = mods[i];
                auto contents = std::make_shared<BpContents>(nullptr, mod_name, BpContents::Type::LEAF, BpContents::LeafType::GRAPH);
                _contents->AddChild(contents);
            }
        }
        return true;
    }

    std::shared_ptr<BpContents> GetContents() { return _contents; }

    Json::Value GetGraphDesc(const std::string& name) {
        if (_mods.find(name) == _mods.end()) {
            return Json::Value();
        }
        return _mods[name];
    }

protected:
    std::vector<Json::Value> LoadGraphMod(const std::string& conf) {
        std::ifstream ifs(conf);
        if (!ifs.is_open()) {
            LOG(ERROR) << "Open \"" << conf << "\" failed";
            return std::vector<Json::Value>();
        }
        Json::Value root;
        Json::Reader reader(Json::Features::strictMode());
        if (!reader.parse(ifs, root)) {
            LOG(ERROR) << "Parse \"" << conf << "\" failed";
            return std::vector<Json::Value>();
        }
        LOG(INFO) << "Load mod graph \"" << conf << "\"...";
        std::vector<Json::Value> res;
        Json::Value::Members mem = root.getMemberNames();
        for (auto iter = mem.begin(); iter != mem.end(); ++iter) {
            res.emplace_back(root[*iter]);
        }
        return res;
    }

    std::shared_ptr<BpContents> _contents;
    std::unordered_map<std::string, Json::Value> _mods;
};

} // namespace bp
