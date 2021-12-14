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

namespace bp {

enum class BpModuleFuncType {
    RES1_ARG1,
    RESN_ARG1,
    RES1_ARGN,
    RESN_ARGN,
};

struct BpModuleFunc {
    BpModuleFuncType type;
    std::any func;
};

class BpModule
{
public:
    typedef std::shared_ptr<google::protobuf::Message> pb_msg_t;
    typedef pb_msg_t (*module_create_val_func_t)(const std::string& msg_name);
    typedef pb_msg_t (*module_func_t)(pb_msg_t);
    typedef std::vector<pb_msg_t> (*module_func2_t)(pb_msg_t);
    typedef pb_msg_t (*module_func3_t)(std::vector<pb_msg_t>);
    typedef std::vector<pb_msg_t> (*module_func4_t)(std::vector<pb_msg_t>);

public:
    BpModule() = default;

    bool LoadModule(const std::string& json_path);

    std::shared_ptr<BpContents> GetContents();

    pb_msg_t CreateModuleVal(const std::string& msg_name);

    BpModuleFunc GetModuleFunc(const std::string& func_name);

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
    
    std::unordered_map<std::string, BpModuleFunc> _module_funcs;
    std::unordered_set<std::string> _var_names;
    module_create_val_func_t _create_var_funcs;
    std::shared_ptr<BpContents> _contents;
};

} // namespace bp

#endif