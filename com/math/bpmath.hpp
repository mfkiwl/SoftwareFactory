#ifndef __BPMATH_HPP__
#define __BPMATH_HPP__
#include <memory>
#include <vector>
#include "bpmath.pb.h"

std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name);

std::shared_ptr<::google::protobuf::Message> add_int(std::shared_ptr<::google::protobuf::Message>);
std::shared_ptr<::google::protobuf::Message> add_float(std::vector<std::shared_ptr<::google::protobuf::Message>>);

std::shared_ptr<::google::protobuf::Message> sub_int(std::shared_ptr<::google::protobuf::Message>);

std::shared_ptr<::google::protobuf::Message> sin_float(std::shared_ptr<::google::protobuf::Message>);

#endif