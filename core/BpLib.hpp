#ifndef __BPLIB_HPP__
#define __BPLIB_HPP__
#include <memory>
#include <string>
#include <google/protobuf/message.h>

#include "BpContents.hpp"

namespace bp {
/*
    module:
        string lib_name
        tree   contents
        func   pbmessage create_msg(string)
        func   pbmessage process_func(string)
    contents:
        library
            module1
                func
                val
            module2
                ...
*/
class BpLib
{
public:
    typedef std::shared_ptr<google::protobuf::Message> pb_msg_t;
    typedef pb_msg_t (*module_func_t)(pb_msg_t);
public:

    virtual bool LoadModule(const std::string& json_path) = 0;

    std::shared_ptr<BpContents> GetContents();

    pb_msg_t CreateModuleVal(const std::string& msg_name);

    module_func_t GetModuleFunc(const std::string& func_name);
};

} // namespace bp

#endif