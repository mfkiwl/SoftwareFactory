#include <unordered_map>
#include <functional>
#include "bpbase.hpp"

static std::unordered_map<std::string, std::function<std::shared_ptr<::google::protobuf::Message>()>> g_msg_map = {
    {"bpbase.Bool", [](){ return std::make_shared<bp::Bool>(); }},
    {"bpbase.Int", [](){ return std::make_shared<bp::Int>(); }},
    {"bpbase.Float", [](){ return std::make_shared<bp::Float>(); }},
    {"bpbase.String", [](){ return std::make_shared<bp::String>(); }},
    {"bpbase.Bytes", [](){ return std::make_shared<bp::Bytes>(); }},
    {"bpbase.IntPair", [](){ return std::make_shared<bp::IntPair>(); }},
    {"bpbase.FloatPair", [](){ return std::make_shared<bp::FloatPair>(); }},
    {"bpbase.IntAry", [](){ return std::make_shared<bp::IntAry>(); }},
    {"bpbase.FloatAry", [](){ return std::make_shared<bp::FloatAry>(); }},
};

std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name) {
    if (g_msg_map.find(msg_name) == g_msg_map.end()) {
        return nullptr;
    }
    return g_msg_map[msg_name]();
}
