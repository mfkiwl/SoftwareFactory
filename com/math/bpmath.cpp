#include <unordered_map>
#include <functional>
#include "bpmath.hpp"
#include "bpbase.pb.h"

static std::unordered_map<std::string, std::function<std::shared_ptr<::google::protobuf::Message>()>> g_msg_map = {
    {"bp.math.BpIntPair", [](){ return std::make_shared<bp_pb::BpIntPair>(); }}
};

std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name) {
    if (g_msg_map.find(msg_name) == g_msg_map.end()) {
        return nullptr;
    }
    return g_msg_map[msg_name]();
}

std::shared_ptr<::google::protobuf::Message> add_int(std::shared_ptr<::google::protobuf::Message> msg) {
    auto nums = std::dynamic_pointer_cast<::bp_pb::BpIntPair>(msg);
    auto res = std::make_shared<bp_pb::BpInt>();
    res->set_var(nums->a() + nums->b());
    return res;
}

std::shared_ptr<::google::protobuf::Message> sub_int(std::shared_ptr<::google::protobuf::Message> msg) {
    auto nums = std::dynamic_pointer_cast<::bp_pb::BpIntPair>(msg);
    auto res = std::make_shared<bp_pb::BpInt>();
    res->set_var(nums->a() - nums->b());
    return res;
}
