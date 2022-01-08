#include <filesystem>
#include <gtest/gtest.h>
#include <glog/logging.h>

#include "bpmath.pb.h"
#include "bpbase.pb.h"

#include "../BpContents.hpp"
#include "../BpModule.hpp"
#include "../BpModuleLinux.hpp"
#include "../BpModLibLinux.hpp"
#include "../BpGraph.hpp"
#include "../Bp.hpp"
#include "../BpVariable.hpp"

TEST(bpcore, BpContents) {
    auto bpcontents = std::make_shared<BpContents>(nullptr, BpContents::Type::VAL, "pb_int_a");
    EXPECT_FALSE(bpcontents->SetParent(nullptr));
    EXPECT_FALSE(bpcontents->AddChild(nullptr));
    EXPECT_TRUE(bpcontents->IsVal());
    EXPECT_EQ(bpcontents->GetFullPath(), "pb_int_a");

    /*
        bp1
        |--bp2
        |--bp3
        |  |--bp4
    */
    auto bp1 = std::make_shared<BpContents>(nullptr, BpContents::Type::CONTENTS, "bp");
    auto bp2 = std::make_shared<BpContents>(nullptr, BpContents::Type::FUNC, "global_add");
    auto bp3 = std::make_shared<BpContents>(nullptr, BpContents::Type::CONTENTS, "math");
    auto bp4 = std::make_shared<BpContents>(nullptr, BpContents::Type::VAL, "pb_int_a");
    bp1->AddChild(bp2);
    EXPECT_EQ(bp2->GetFullPath(), "bp.global_add");
    bp3->SetParent(bp1);
    bp4->SetParent(bp3);
    EXPECT_EQ(bp4->GetFullPath(), "bp.math.pb_int_a");

    LOG(INFO) << "\n" << bp1->PrintContents();
}

TEST(bpcore, BpVariable) {
    std::weak_ptr<bp_pb::BpInt> w_msg;
    std::weak_ptr<bp_pb::BpInt> w_msg2;
    std::weak_ptr<bp_pb::BpInt> w_msg3;
    {
        auto msg = std::make_shared<bp_pb::BpInt>();
        bp::BpVariable var("", "", msg);
        w_msg = msg;
        EXPECT_FALSE(w_msg.expired());
        // 设置值传递
        auto msg_tmp = std::make_shared<bp_pb::BpInt>();
        msg_tmp->set_var(100);
        bp::BpVariable var2("", "", msg_tmp);
        auto msg2 = std::make_shared<bp_pb::BpInt>();
        msg2->set_var(200);
        w_msg2 = msg2;
        var2.SetValue(msg2);
        msg2->set_var(300);
        EXPECT_EQ(300, msg2->var());
        EXPECT_EQ(200, var2.Get<bp_pb::BpInt>()->var());
        // 设置引用传递
        auto msg3 = std::make_shared<bp_pb::BpInt>();
        w_msg3 = msg3;
        msg3->set_var(100);
        bp::BpVariable var3;
        var3.SetValueByRef(msg3);
        msg3->set_var(200);
        EXPECT_EQ(200, var3.Get<bp_pb::BpInt>()->var());
    }
    EXPECT_TRUE(w_msg.expired());
    EXPECT_TRUE(w_msg2.expired());
    EXPECT_TRUE(w_msg3.expired());
}

TEST(bpcore, BpModule) {
    auto cur_path = std::filesystem::current_path().string();
    bp::BpModuleLinux bml;
    EXPECT_FALSE(bml.LoadModule(""));
    EXPECT_TRUE(bml.LoadModule(cur_path + "/../conf/bpmath.json"));
    LOG(INFO) << "bpmath.json contents:";
    LOG(INFO) << "\n" << bml.GetContents()->PrintContents();

    auto pbmsg = bml.CreateModuleVal("bpmath.BpIntPair");
    EXPECT_TRUE(nullptr != pbmsg);
    auto pbintpair = std::static_pointer_cast<::bp_pb::BpIntPair>(pbmsg);
    pbintpair->set_a(100);

    {
        auto func = bml.GetModuleFunc("add_int");
        auto p = std::make_shared<::bp_pb::BpIntPair>();
        p->set_a(1);
        p->set_b(1);
        auto res = std::any_cast<module_func2_t>(func.func)(p);
        EXPECT_EQ(std::static_pointer_cast<::bp_pb::BpInt>(res)->var(), 2);
    }
    {
        auto func = bml.GetModuleFunc("sub_int");
        auto p = std::make_shared<::bp_pb::BpIntPair>();
        p->set_a(1);
        p->set_b(1);
        auto res = std::any_cast<module_func2_t>(func.func)(p);
        EXPECT_EQ(std::static_pointer_cast<::bp_pb::BpInt>(res)->var(), 0);
    }
}

TEST(bpcore, BpModLib) {
    auto cur_path = std::filesystem::current_path().string();
    bp::BpModLibLinux bll;
    EXPECT_TRUE(bll.Init(cur_path + "/../conf/"));
    EXPECT_TRUE(nullptr != bll.CreateVal("bpmath.BpIntPair"));
    auto func = bll.GetFunc("bpmath.add_int");
    EXPECT_FALSE(func.type == bp::BpModuleFuncType::UNKNOWN);
}

TEST(bpcore, Bp) {
    auto& b = bp::Bp::Instance();
    LOG(INFO) << "bp version: " << bp::Bp::Instance().Version();
    auto g = std::make_shared<bp::BpGraph>();
    EXPECT_EQ(bp::LoadState::ERR_OPEN_FILE, b.LoadGraph(std::string(""), g));
    EXPECT_NE(bp::LoadState::OK, b.LoadGraph(std::string("../conf/com_random.json"), g));
}

TEST(bpcore, BpGraph) {
    auto g = std::make_shared<bp::BpGraph>(nullptr);
   
}