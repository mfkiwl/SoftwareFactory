#ifndef __BPBASE_HPP__
#define __BPBASE_HPP__
#include <memory>
#include "bpbase.pb.h"

namespace bp {
namespace base {

    std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name);
    
} // namespace base
} // namespace bp

#endif