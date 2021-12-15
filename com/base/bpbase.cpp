#include <unordered_set>
#include <functional>
#include "bpbase.hpp"

static std::unordered_map<std::string, std::function<std::shared_ptr<::google::protobuf::Message>()>> g_msg_map = {
    {"bp.base.BpInt", [](){ return std::make_shared<bp_pb::BpInt>(); }},
    {"bp.base.BpFloat", [](){ return std::make_shared<bp_pb::BpFloat>(); }},
    {"bp.base.BpString", [](){ return std::make_shared<bp_pb::BpString>(); }},
};

std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name) {
    if (g_msg_map.find(msg_name) == g_msg_map.end()) {
        return nullptr;
    }
    return g_msg_map[msg_name]();
}
