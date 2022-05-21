#include <cmath>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "bpbase.pb.h"

typedef ::google::protobuf::Message pb_msg_t;
typedef std::shared_ptr<pb_msg_t> pb_msg_ptr_t;

static std::unordered_map<std::string, std::function<pb_msg_ptr_t()>> g_msg_map = {
    // {"bpmath.BpIntPair", [](){ return std::make_shared<bp::IntPair>(); }}
};

pb_msg_ptr_t create_msg(const std::string& msg_name) {
    if (g_msg_map.find(msg_name) == g_msg_map.end()) {
        return nullptr;
    }
    return g_msg_map[msg_name]();
}

//// type convert
///// to bool
pb_msg_ptr_t int32_to_bool(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Int32>(msg);
    auto res = std::make_shared<bp::Bool>();
    res->set_var(a->var());
    return res;
}
pb_msg_ptr_t uint32_to_bool(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::UInt32>(msg);
    auto res = std::make_shared<bp::Bool>();
    res->set_var(a->var());
    return res;
}
pb_msg_ptr_t float_to_bool(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Float>(msg);
    auto res = std::make_shared<bp::Bool>();
    res->set_var(a->var());
    return res;
}
///// to int32
pb_msg_ptr_t bool_to_int32(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Bool>(msg);
    auto res = std::make_shared<bp::Int32>();
    res->set_var(a->var() ? 1 : 0);
    return res;
}
pb_msg_ptr_t uint32_to_int32(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::UInt32>(msg);
    auto res = std::make_shared<bp::Int32>();
    res->set_var((int32_t)a->var());
    return res;
}
pb_msg_ptr_t float_to_int32(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Float>(msg);
    auto res = std::make_shared<bp::Int32>();
    res->set_var((int32_t)a->var());
    return res;
}
///// to uint32
pb_msg_ptr_t bool_to_uint32(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Bool>(msg);
    auto res = std::make_shared<bp::UInt32>();
    res->set_var(a->var() ? 1 : 0);
    return res;
}
pb_msg_ptr_t int32_to_uint32(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Int32>(msg);
    auto res = std::make_shared<bp::UInt32>();
    res->set_var((uint32_t)a->var());
    return res;
}
pb_msg_ptr_t float_to_uint32(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Float>(msg);
    auto res = std::make_shared<bp::UInt32>();
    res->set_var((uint32_t)a->var());
    return res;
}
///// to float
pb_msg_ptr_t bool_to_float(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Bool>(msg);
    auto res = std::make_shared<bp::Float>();
    res->set_var(a->var() ? 1.0 : 0.0);
    return res;
}
pb_msg_ptr_t uint32_to_float(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::UInt32>(msg);
    auto res = std::make_shared<bp::Float>();
    res->set_var((float)a->var());
    return res;
}
pb_msg_ptr_t int32_to_float(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Int32>(msg);
    auto res = std::make_shared<bp::Float>();
    res->set_var((float)a->var());
    return res;
}

//// bool
pb_msg_ptr_t logic_or_bool(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Bool>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Bool>(msgs[1]);
    auto res = std::make_shared<bp::Bool>();
    res->set_var(a->var() || b->var());
    return res;
}
pb_msg_ptr_t logic_and_bool(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Bool>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Bool>(msgs[1]);
    auto res = std::make_shared<bp::Bool>();
    res->set_var(a->var() && b->var());
    return res;
}
pb_msg_ptr_t logic_not_bool(pb_msg_ptr_t msg) {
    auto a = std::dynamic_pointer_cast<::bp::Bool>(msg);
    auto res = std::make_shared<bp::Bool>();
    res->set_var(!a->var());
    return res;
}
pb_msg_ptr_t equal_bool(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Bool>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Bool>(msgs[1]);
    auto res = std::make_shared<bp::Bool>();
    res->set_var(a->var() == b->var());
    return res;
}

//// int32
pb_msg_ptr_t add_int32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Int32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Int32>(msgs[1]);
    auto res = std::make_shared<bp::Int32>();
    res->set_var(a->var() + b->var());
    return res;
}
pb_msg_ptr_t sub_int32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Int32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Int32>(msgs[1]);
    auto res = std::make_shared<bp::Int32>();
    res->set_var(a->var() - b->var());
    return res;
}
pb_msg_ptr_t mut_int32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Int32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Int32>(msgs[1]);
    auto res = std::make_shared<bp::Int32>();
    res->set_var(a->var() * b->var());
    return res;
}
pb_msg_ptr_t div_int32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Int32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Int32>(msgs[1]);
    auto res = std::make_shared<bp::Int32>();
    res->set_var(a->var() / b->var());
    return res;
}
pb_msg_ptr_t equal_int32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Int32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Int32>(msgs[1]);
    auto res = std::make_shared<bp::Bool>();
    res->set_var(a->var() == b->var());
    return res;
}

//// uint32
pb_msg_ptr_t add_uint32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::UInt32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::UInt32>(msgs[1]);
    auto res = std::make_shared<bp::UInt32>();
    res->set_var(a->var() + b->var());
    return res;
}
pb_msg_ptr_t sub_uint32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::UInt32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::UInt32>(msgs[1]);
    auto res = std::make_shared<bp::UInt32>();
    res->set_var(a->var() - b->var());
    return res;
}
pb_msg_ptr_t mut_uint32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::UInt32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::UInt32>(msgs[1]);
    auto res = std::make_shared<bp::UInt32>();
    res->set_var(a->var() * b->var());
    return res;
}
pb_msg_ptr_t div_uint32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::UInt32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::UInt32>(msgs[1]);
    auto res = std::make_shared<bp::UInt32>();
    res->set_var(a->var() / b->var());
    return res;
}
pb_msg_ptr_t equal_uint32(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::UInt32>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::UInt32>(msgs[1]);
    auto res = std::make_shared<bp::Bool>();
    res->set_var(a->var() == b->var());
    return res;
}

//// float
pb_msg_ptr_t add_float(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Float>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Float>(msgs[1]);
    auto res = std::make_shared<bp::Float>();
    res->set_var(a->var() + b->var());
    return res;
}
pb_msg_ptr_t sub_float(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Float>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Float>(msgs[1]);
    auto res = std::make_shared<bp::Float>();
    res->set_var(a->var() - b->var());
    return res;
}
pb_msg_ptr_t mut_float(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Float>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Float>(msgs[1]);
    auto res = std::make_shared<bp::Float>();
    res->set_var(a->var() * b->var());
    return res;
}
pb_msg_ptr_t div_float(std::vector<pb_msg_ptr_t> msgs) {
    auto a = std::dynamic_pointer_cast<::bp::Float>(msgs[0]);
    auto b = std::dynamic_pointer_cast<::bp::Float>(msgs[1]);
    auto res = std::make_shared<bp::Float>();
    res->set_var(a->var() / b->var());
    return res;
}

//// sin, cos, tan
pb_msg_ptr_t sin_float(pb_msg_ptr_t msg) {
    auto nums = std::dynamic_pointer_cast<::bp::Float>(msg);
    auto res = std::make_shared<bp::Float>();
    res->set_var(std::sin(nums->var()));
    return res;
}
pb_msg_ptr_t cos_float(pb_msg_ptr_t msg) {
    auto nums = std::dynamic_pointer_cast<::bp::Float>(msg);
    auto res = std::make_shared<bp::Float>();
    res->set_var(std::cos(nums->var()));
    return res;
}
pb_msg_ptr_t tan_float(pb_msg_ptr_t msg) {
    auto nums = std::dynamic_pointer_cast<::bp::Float>(msg);
    auto res = std::make_shared<bp::Float>();
    res->set_var(std::tan(nums->var()));
    return res;
}
