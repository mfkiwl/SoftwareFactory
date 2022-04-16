#include <gtest/gtest.h>
#include <glog/logging.h>

#include "../SFEParams.hpp"

const char* const json_str = 
    "{\n"
    "    \"system\": {\n"
    "        \"window\":{\n"
    "            \"width\":1280,\n"
    "            \"height\":720\n"
    "        }\n"
    "    },\n"
    "    \"panel\":{\n"
    "        \"log\":{\n"
    "            \"show\":true\n"
    "        },\n"
    "        \"mainmenu\":{\n"
    "            \"show\":true\n"
    "        },\n"
    "        \"bp editor\":{\n"
    "            \"show\":true\n"
    "        },\n"
    "        \"lib\":{\n"
    "            \"show\":true\n"
    "        },\n"
    "        \"graph\":{\n"
    "            \"show\":true\n"
    "        },\n"
    "        \"plot\":{\n"
    "            \"show\":true\n"
    "        }\n"
    "    }\n"
    "}\n";

TEST(SFEParams, splitkeys) {
    auto res = sfe::SFEParams::Instance().SplitKeys("system.show");
    for (int i = 0; i < res.size(); ++i) {
        LOG(INFO) << res[i];
    }
    EXPECT_EQ(2, res.size());
}

TEST(SFEParams, params_get_set) {
    auto& params = sfe::SFEParams::Instance();
    params.LoadParams(json_str);
    EXPECT_EQ(params.GetParam("system.window.width").asInt(), 1280);
    params.SetParam("system.window.width", 110);
    EXPECT_EQ(params.GetParam("system.window.width").asInt(), 110);
    LOG(INFO) << params.ToStr();
    EXPECT_TRUE(params.SetParam("system.window.dg.xg.ui.dd", 110));
    LOG(INFO) << params.ToStr();
}