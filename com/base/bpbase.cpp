#include <unordered_set>
#include <functional>
#include "bpbase.hpp"

static std::unordered_map<std::string, std::function<std::shared_ptr<::google::protobuf::Message>()>> g_msg_map = {
    {"bpbase.BpBool", [](){ return std::make_shared<bp_pb::BpBool>(); }},
    {"bpbase.BpInt", [](){ return std::make_shared<bp_pb::BpInt>(); }},
    {"bpbase.BpFloat", [](){ return std::make_shared<bp_pb::BpFloat>(); }},
    {"bpbase.BpString", [](){ return std::make_shared<bp_pb::BpString>(); }},
    {"bpbase.BpBytes", [](){ return std::make_shared<bp_pb::BpBytes>(); }},
    {"bpbase.BpIntPair", [](){ return std::make_shared<bp_pb::BpIntPair>(); }},
    {"bpbase.BpFloatPair", [](){ return std::make_shared<bp_pb::BpFloatPair>(); }},
    {"bpbase.BpIntAry", [](){ return std::make_shared<bp_pb::BpIntAry>(); }},
    {"bpbase.BpFloatAry", [](){ return std::make_shared<bp_pb::BpFloatAry>(); }},
};

std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name) {
    if (g_msg_map.find(msg_name) == g_msg_map.end()) {
        return nullptr;
    }
    return g_msg_map[msg_name]();
}
