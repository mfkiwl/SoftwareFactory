#include <cmath>
#include <memory>
#include <functional>
#include <unordered_map>

#include "bpbase.pb.h"

static std::unordered_map<std::string, std::function<std::shared_ptr<::google::protobuf::Message>()>> g_msg_map = {
    // {"bpmath.String", [](){ return std::make_shared<bp::String>(); }}
};

std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name) {
    if (g_msg_map.find(msg_name) == g_msg_map.end()) {
        return nullptr;
    }
    return g_msg_map[msg_name]();
}

std::shared_ptr<::google::protobuf::Message> print_hello() {
    auto res = std::make_shared<bp::String>();
    res->set_var("hello,world");
    return res;
}
