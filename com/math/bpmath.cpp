#include <cmath>
#include <unordered_map>
#include <functional>
#include "bpmath.hpp"
#include "bpbase.pb.h"

static std::unordered_map<std::string, std::function<std::shared_ptr<::google::protobuf::Message>()>> g_msg_map = {
    // {"bpmath.BpIntPair", [](){ return std::make_shared<bp::IntPair>(); }}
};

std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name) {
    if (g_msg_map.find(msg_name) == g_msg_map.end()) {
        return nullptr;
    }
    return g_msg_map[msg_name]();
}

std::shared_ptr<::google::protobuf::Message> add_int(std::shared_ptr<::google::protobuf::Message> msg) {
    auto nums = std::dynamic_pointer_cast<::bp::IntPair>(msg);
    auto res = std::make_shared<bp::Int>();
    res->set_var(nums->a() + nums->b());
    return res;
}
std::shared_ptr<::google::protobuf::Message> add_float(std::vector<std::shared_ptr<::google::protobuf::Message>> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Float>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Float>(msgs[1]);
    auto res = std::make_shared<bp::Float>();
    res->set_var(a->var() + b->var());
    return res;
}

std::shared_ptr<::google::protobuf::Message> sub_int(std::shared_ptr<::google::protobuf::Message> msg) {
    auto nums = std::dynamic_pointer_cast<::bp::IntPair>(msg);
    auto res = std::make_shared<bp::Int>();
    res->set_var(nums->a() - nums->b());
    return res;
}

std::shared_ptr<::google::protobuf::Message> sin_float(std::shared_ptr<::google::protobuf::Message> msg) {
    auto nums = std::dynamic_pointer_cast<::bp::Float>(msg);
    auto res = std::make_shared<bp::Float>();
    res->set_var(std::sin(nums->var()));
    return res;
}
