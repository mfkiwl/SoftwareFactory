#include <cmath>
#include <memory>
#include <functional>
#include <unordered_map>

#include "SFEPanel.hpp"
#include "bpbase.pb.h"

typedef ::google::protobuf::Message pb_msg_t;
typedef std::shared_ptr<pb_msg_t> pb_msg_ptr_t;

static std::unordered_map<std::string, std::function<pb_msg_ptr_t()>> g_msg_map = {
    // {"bpmath.String", [](){ return std::make_shared<bp::String>(); }}
};

pb_msg_ptr_t create_msg(const std::string& msg_name) {
    if (g_msg_map.find(msg_name) == g_msg_map.end()) {
        return nullptr;
    }
    return g_msg_map[msg_name]();
}

pb_msg_ptr_t print_hello() {
    auto res = std::make_shared<bp::Int>();
    sfe::SFEPanel::SendMessage("uinode_print_hello", "log", 
        std::string("++++++++++++++++++++++++ hello, world\n"));
    res->set_var(0);
    return res;
}
