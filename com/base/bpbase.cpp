#include <memory>
#include <unordered_map>
#include <functional>

#include "bpbase.pb.h"

typedef ::google::protobuf::Message pb_msg_t;
typedef std::shared_ptr<pb_msg_t> pb_msg_ptr_t;

static std::unordered_map<std::string, std::function<pb_msg_ptr_t()>> g_msg_map = {
    {"bpbase.Bool", [](){ return std::make_shared<bp::Bool>(); }},
    {"bpbase.Int32", [](){ return std::make_shared<bp::Int32>(); }},
    {"bpbase.UInt32", [](){ return std::make_shared<bp::UInt32>(); }},
    {"bpbase.Float", [](){ return std::make_shared<bp::Float>(); }},
    {"bpbase.String", [](){ return std::make_shared<bp::String>(); }},
    {"bpbase.Bytes", [](){ return std::make_shared<bp::Bytes>(); }},
    {"bpbase.Int32Ary", [](){ return std::make_shared<bp::Int32Ary>(); }},
    {"bpbase.UInt32Ary", [](){ return std::make_shared<bp::UInt32Ary>(); }},
    {"bpbase.FloatAry", [](){ return std::make_shared<bp::FloatAry>(); }},
};

pb_msg_ptr_t create_msg(const std::string& msg_name) {
    if (g_msg_map.find(msg_name) == g_msg_map.end()) {
        return nullptr;
    }
    return g_msg_map[msg_name]();
}
