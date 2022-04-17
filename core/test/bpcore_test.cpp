#include <set>
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
#include "evnode/BpNodeEv.hpp"

TEST(bpcore, BpNodeEv) {
    auto begin_node = std::make_shared<bp::BpNodeEv>("Begin", nullptr, 1, 1);
    auto tick_node = std::make_shared<bp::BpNodeEv>("Tick", nullptr, 2, -1);

    std::set<std::shared_ptr<bp::BpNodeEv>, bp::BpNodeEvCmp> ev_node_set;
    ev_node_set.insert(begin_node);
    ev_node_set.insert(tick_node);
    for (const auto& it : ev_node_set) {
        LOG(INFO) << "lv: " << it->GetName();
    }
}

TEST(bpcore, BpContents) {
    auto bpcontents = std::make_shared<BpContents>(nullptr, "pb_int_a", BpContents::Type::LEAF, BpContents::LeafType::VAL);
    EXPECT_FALSE(bpcontents->SetParent(nullptr));
    EXPECT_FALSE(bpcontents->AddChild(nullptr));
    EXPECT_EQ(bpcontents->GetFullPath(), "pb_int_a");

    /*
        bp1
        |--bp2
        |--bp3
        |  |--bp4
    */
    auto bp1 = std::make_shared<BpContents>(nullptr, "bp", BpContents::Type::CONTENTS);
    auto bp2 = std::make_shared<BpContents>(nullptr, "global_add", BpContents::Type::LEAF, BpContents::LeafType::FUNC);
    auto bp3 = std::make_shared<BpContents>(nullptr, "math", BpContents::Type::CONTENTS);
    auto bp4 = std::make_shared<BpContents>(nullptr, "pb_int_a", BpContents::Type::LEAF, BpContents::LeafType::VAL);
    bp1->AddChild(bp2);
    EXPECT_EQ(bp2->GetFullPath(), "bp.global_add");
    bp3->SetParent(bp1);
    bp4->SetParent(bp3);
    EXPECT_EQ(bp4->GetFullPath(), "bp.math.pb_int_a");

    LOG(INFO) << "\n" << bp1->PrintContents();
}

TEST(bpcore, BpVariable) {
    std::weak_ptr<bp::Int32> w_msg;
    std::weak_ptr<bp::Int32> w_msg2;
    std::weak_ptr<bp::Int32> w_msg3;
    {
        auto msg = std::make_shared<bp::Int32>();
        bp::BpVariable var("", "", msg);
        w_msg = msg;
        EXPECT_FALSE(w_msg.expired());
        // 设置值传递
        auto msg_tmp = std::make_shared<bp::Int32>();
        msg_tmp->set_var(100);
        bp::BpVariable var2("", "", msg_tmp);
        auto msg2 = std::make_shared<bp::Int32>();
        msg2->set_var(200);
        w_msg2 = msg2;
        var2.SetValue(msg2);
        msg2->set_var(300);
        EXPECT_EQ(300, msg2->var());
        EXPECT_EQ(200, var2.Get<bp::Int32>()->var());
        // 设置引用传递
        auto msg3 = std::make_shared<bp::Int32>();
        w_msg3 = msg3;
        msg3->set_var(100);
        bp::BpVariable var3;
        var3.SetValueByRef(msg3);
        msg3->set_var(200);
        EXPECT_EQ(200, var3.Get<bp::Int32>()->var());
    }
    EXPECT_TRUE(w_msg.expired());
    EXPECT_TRUE(w_msg2.expired());
    EXPECT_TRUE(w_msg3.expired());
}

TEST(bpcore, BpModule) {
    bp::BpModuleLinux bml;
    EXPECT_FALSE(bml.LoadModule(""));
    EXPECT_TRUE(bml.LoadModule("../conf/bpmath.json"));
    EXPECT_TRUE(bml.LoadModule("../conf/bpbase.json"));
    LOG(INFO) << "bpmath.json contents:";
    LOG(INFO) << "\n" << bml.GetContents()->PrintContents();

    auto pbmsg = bml.CreateModuleVal("bpbase.Int32");
    EXPECT_TRUE(nullptr != pbmsg);
    {
        auto func = bml.GetModuleFunc("add_int32");
        auto a = std::make_shared<::bp::Int32>();
        auto b = std::make_shared<::bp::Int32>();
        a->set_var(1);
        b->set_var(1);
        std::vector<pb_msg_ptr_t> args;
        args.push_back(a);
        args.push_back(b);
        auto res = reinterpret_cast<module_func4_t>(func.func)(args);
        EXPECT_EQ(std::static_pointer_cast<::bp::Int32>(res)->var(), 2);
    }
}

TEST(bpcore, BpModLib) {
    bp::BpModLibLinux bll;
    EXPECT_TRUE(bll.Init("../conf/"));
    EXPECT_TRUE(nullptr != bll.CreateVal("bpbase.IntPair").var);
    auto func = bll.GetFunc("bpmath.add_int");
    EXPECT_FALSE(func.type == bp::BpModuleFuncType::UNKNOWN);
}

TEST(bpcore, Bp) {
    auto& b = bp::Bp::Instance();
    auto g = std::make_shared<bp::BpGraph>();
    Json::Value nodes_pos;
    EXPECT_EQ(bp::LoadSaveState::ERR_OPEN_FILE, b.LoadGraph(std::string(""), g, nodes_pos));
    EXPECT_NE(bp::LoadSaveState::OK, b.LoadGraph(std::string("../conf/com_random.json"), g, nodes_pos));
}