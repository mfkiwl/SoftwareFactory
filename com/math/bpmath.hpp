#ifndef __BPMATH_HPP__
#define __BPMATH_HPP__
#include <memory>
#include "bpmath.pb.h"

namespace bp {
namespace math { 

    std::shared_ptr<::google::protobuf::Message> create_msg(const std::string& msg_name);

    std::shared_ptr<::google::protobuf::Message> add_int(std::shared_ptr<::google::protobuf::Message>);

} //namespace math
} // namespace bp

#endif