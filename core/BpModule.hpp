#ifndef __BP_MODULE_HPP__
#define __BP_MODULE_HPP__
#include <any>
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <google/protobuf/message.h>
#include <jsoncpp/json/json.h>
#include "BpContents.hpp"

typedef google::protobuf::Message pb_msg_t;
typedef std::shared_ptr<google::protobuf::Message> pb_msg_ptr_t;
typedef pb_msg_ptr_t (*module_create_val_func_t)(const std::string& var_name);
typedef pb_msg_ptr_t (*module_func0_t)();
typedef std::vector<pb_msg_ptr_t> (*module_func1_t)();
typedef pb_msg_ptr_t (*module_func2_t)(pb_msg_ptr_t);
typedef std::vector<pb_msg_ptr_t> (*module_func3_t)(pb_msg_ptr_t);
typedef pb_msg_ptr_t (*module_func4_t)(std::vector<pb_msg_ptr_t>);
typedef std::vector<pb_msg_ptr_t> (*module_func5_t)(std::vector<pb_msg_ptr_t>);

namespace bp {

enum class BpModuleFuncType {
    UNKNOWN,
    RES1_ARG0,
    RESN_ARG0,
    RES1_ARG1,
    RESN_ARG1,
    RES1_ARGN,
    RESN_ARGN,
};

struct BpModuleVar {
    pb_msg_ptr_t var = nullptr;
    std::string desc = "";
};

struct BpModuleFunc {
    BpModuleFuncType type = BpModuleFuncType::UNKNOWN;
    std::string name;
    std::vector<std::string> type_args;
    std::vector<std::string> name_args;
    std::vector<std::string> type_res;
    std::vector<std::string> name_res;
    std::any func;
};

class BpModule
{
public:
    BpModule() = default;
    virtual ~BpModule() {
        _module_funcs.clear();
    }

    bool LoadModule(const std::string& json_path);

    std::shared_ptr<BpContents> GetContents();

    pb_msg_ptr_t CreateModuleVal(const std::string& var_name);
    const std::string GetModuleVarDesc(const std::string& var_name);

    BpModuleFunc GetModuleFunc(const std::string& func_name);

    const std::string& Name() {
        return _mod_name;
    }
protected:
    /*
        从配置文件获得动态库路径, 并读取动态库符号表, 存储符号表内容
    */
    virtual bool Init(const char* dll_path) = 0;

    /*
        从_module_symbols模糊搜索func_name对应的字符串并加载该函数
    */
    virtual void* GetFunc(const std::string& func_name) = 0;

    void BuildContents(Json::Value&, std::shared_ptr<BpContents>);
    void AddFunc(std::string&, Json::Value&, void*);
    
    std::unordered_map<std::string, BpModuleFunc> _module_funcs;
    std::unordered_map<std::string, std::string> _var_names;
    module_create_val_func_t _create_var_funcs = nullptr;
    std::shared_ptr<BpContents> _contents;
    std::string _mod_name;
};

} // namespace bp

#endif