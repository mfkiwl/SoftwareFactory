#pragma once
#include <memory>
#include <vector>
#include "bpbase.pb.h"

std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name);

std::shared_ptr<::google::protobuf::Message> print_hello();
