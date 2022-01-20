#include <assert.h>
#include <fstream>
#include <filesystem>
#include <glog/logging.h>
#include "BpModule.hpp"

namespace bp {

/*
    生成contents树目录
    生成_module_funcs
    生成_create_var_funcs
*/
bool BpModule::LoadModule(const std::string& json_file) {
    // 使用module名作为根节点
    // 搜索遍历命名区域
    //   搜索命名域下的函数和变量
    LOG(INFO) << "begin load \"" << json_file << "\"...";
    std::ifstream ifs;
    ifs.open(json_file);
    if (!ifs.is_open()) {
        LOG(ERROR) << "open \"" << json_file << "\" failed";
        return false;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false)) {
        LOG(ERROR) << "parse \"" << json_file << "\" failed";
        return false;
    }
    ifs.close();

    if (!root.isMember("_lib")) {
        LOG(ERROR) << json_file <<" has no key \"_lib\"";
        return false;
    }
    // 提取模块名称
    _mod_name = root["_lib"].asString();
    _mod_name = _mod_name.substr(3, _mod_name.size() - 6);
    _contents = std::make_shared<BpContents>(nullptr, _mod_name, BpContents::Type::CONTENTS);
    // 初始化dll
    std::filesystem::path p(json_file);
    std::string dll_path = p.parent_path().string() + "/../lib/" + root["_lib"].asString();
    if (!Init(dll_path.c_str())) {
        LOG(ERROR) << "load dll " << dll_path << " failed";
        return false;
    }
    // 加载创建变量函数
    auto f = GetFunc("create_msg");
    if (f == nullptr) {
        LOG(ERROR) << "find " << _mod_name << " \"create_msg\" func failed";
    } else {
        _create_var_funcs = reinterpret_cast<module_create_val_func_t>(f);
    }
    BuildContents(root, _contents);
    LOG(INFO) << "load \"" << json_file << "\": \n" << _contents->PrintContents();
    return true;
}

void BpModule::BuildContents(Json::Value& v, std::shared_ptr<BpContents> contents) {
    Json::Value::Members mem = v.getMemberNames();
	Json::Value::Members::iterator it;
	for (it = mem.begin(); it != mem.end(); it++) {
        if (!(*it).empty() && (*it)[0] == '_') {
            if ((*it) == "_func") {
                Json::Value::Members func_mem = v[*it].getMemberNames();
	            Json::Value::Members::iterator func_it;
                for (func_it = func_mem.begin(); func_it != func_mem.end(); func_it++) {
                    auto func = v[*it][*func_it];
                    auto leaf_func = std::make_shared<BpContents>(contents, (*func_it), BpContents::Type::LEAF, BpContents::LeafType::FUNC);
                    // 加载函数符号
                    auto f = GetFunc(*func_it);
                    if (f == nullptr) {
                        LOG(ERROR) << "find func " << *func_it << " failed, skip";
                        continue;
                    }
                    // 函数符号添加到_module_funcs中
                    AddFunc(*func_it, func, f);
                    contents->AddChild(leaf_func);
                }
            }
            if ((*it) == "_val") {
                if (!v[*it].isArray()) {
                    LOG(ERROR) << "_val is nor array: " << v[*it].toStyledString();
                    continue;
                }
                for (int i = 0; i < v[*it].size(); ++i) {
                    auto leaf_val = std::make_shared<BpContents>(contents, v[*it][i].asString(), BpContents::Type::LEAF, BpContents::LeafType::VAL);
                    contents->AddChild(leaf_val);
                    _var_names.insert(v[*it][i].asString());
                }
            }
        } else {
            auto child = std::make_shared<BpContents>(contents, *it, BpContents::Type::CONTENTS);
            BuildContents(v[*it], child);
        }
    }
}

void BpModule::AddFunc(std::string& func_name, Json::Value& v, void* func) {
    BpModuleFunc f;
    int in_n = v["_input"].size();
    int out_n = v["_output"].size();
    for (int i = 0; i < in_n; ++i) {
        f.type_args.emplace_back(v["_input"][i].asString());
    }
    for (int i = 0; i < in_n; ++i) {
        f.type_res.emplace_back(v["_output"][i].asString());
    }
    if (in_n == 0 && out_n == 1) {
        f.type = BpModuleFuncType::RES1_ARG0;
        f.func = reinterpret_cast<module_func0_t>(func);
    } else if (in_n == 0 && out_n > 1) {
        f.type = BpModuleFuncType::RESN_ARG0;
        f.func = reinterpret_cast<module_func1_t>(func);
    } else if (in_n == 1 && out_n == 1) {
        f.type = BpModuleFuncType::RES1_ARG1;
        f.func = reinterpret_cast<module_func2_t>(func);
    } else if (in_n == 1 && out_n > 1) {
        f.type = BpModuleFuncType::RESN_ARG1;
        f.func = reinterpret_cast<module_func3_t>(func);
    } else if (in_n > 1 && out_n == 1) {
        f.type = BpModuleFuncType::RES1_ARGN;
        f.func = reinterpret_cast<module_func4_t>(func);
    } else if (in_n > 1 && out_n > 1) {
        f.type = BpModuleFuncType::RESN_ARGN;
        f.func = reinterpret_cast<module_func5_t>(func);
    }
    _module_funcs[func_name] = f;
}

std::shared_ptr<BpContents> BpModule::GetContents() {
    return _contents;
}

pb_msg_ptr_t BpModule::CreateModuleVal(const std::string& msg_name) {
    if (_var_names.find(msg_name) == _var_names.end()) {
        LOG(ERROR) << _mod_name << ": can't find var " << msg_name;
        return nullptr;
    }
    if (_create_var_funcs == nullptr) {
        LOG(ERROR) << _mod_name << ": create_msg is nullptr";
        return nullptr;
    }
    return _create_var_funcs(msg_name);
}

BpModuleFunc BpModule::GetModuleFunc(const std::string& func_name) {
    if (_module_funcs.find(func_name) == _module_funcs.end()) {
        LOG(ERROR) << _mod_name << ": cat't find func " << func_name;
        return BpModuleFunc();
    }
    return _module_funcs[func_name];
}

} // namespace bp