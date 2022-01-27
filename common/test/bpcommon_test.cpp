#include <gtest/gtest.h>

#include "../bpcommon.hpp"
#include "bpbase.pb.h"
#include "bpmath.pb.h"

TEST(bpcommon, PbJsonConvert) {
    {
        ::bp_pb::BpInt msg;
        // msg.set_var(100);
        std::string json;
        bp::JsonPbConvert::PbMsg2JsonStr(msg, json);
        std::cout << "json: " << json << std::endl;
        EXPECT_TRUE(bp::JsonPbConvert::JsonStr2PbMsg(json, msg));
        std::cout << "pb: \n{" << msg.DebugString() << "}" << std::endl;
    }

    {
        ::bp_pb::BpIntPair msg;
        Json::Value v;
        bp::JsonPbConvert::PbMsg2Json(msg, v);
        std::cout << "json: " << v.toStyledString() << std::endl;
        v["a"] = 10;
        v["b"] = 100;
        EXPECT_TRUE(bp::JsonPbConvert::Json2PbMsg(v, msg));
        std::cout << "pb: \n{" << msg.DebugString() << "}" << std::endl;
    }

    {
        Json::Value v;
        Json::Value v2;
        v2["1"] = "hello";
        v["a"] = v2;
        Json::FastWriter writer;
        auto dst = writer.write(v);
        std::cout << "fast write: " << dst << std::endl;
        Json::Value::Members mem = v.getMemberNames();
        for (auto iter = mem.begin(); iter != mem.end(); ++iter) {
            std::cout << *iter << std::endl;
            std::cout << v[*iter] << std::endl;
        }
    }

    {
        // test repeate data
        // TODO
        ::bp_pb::BpIntAry msg;
        std::string json;
        bp::JsonPbConvert::PbMsg2JsonStr(msg, json);
        std::cout << "repeat ary: " << json << std::endl;
    }
}
