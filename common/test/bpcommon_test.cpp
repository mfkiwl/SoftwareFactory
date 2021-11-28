#include <gtest/gtest.h>

#include "../bpcommon.hpp"
#include "bpbase.pb.h"

TEST(bpcommon, PbJsonConvert) {
    ::bp_pb::BpInt msg;
    msg.set_var(100);
    std::string json;
    bp::JsonPbConvert::PbMsg2JsonStr(msg, json);
    std::cout << json << std::endl;
    EXPECT_TRUE(bp::JsonPbConvert::JsonStr2PbMsg(json, msg));
    std::cout << msg.DebugString() << std::endl;
}