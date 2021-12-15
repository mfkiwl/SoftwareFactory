#include <filesystem>
#include <gtest/gtest.h>

#include "../BpContents.hpp"
#include "../BpModule.hpp"
#include "../BpModuleLinux.hpp"
#include "../BpLibLinux.hpp"
#include "bpmath.pb.h"
#include "bpbase.pb.h"

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
}

TEST(bpcore, BpModule) {
    auto cur_path = std::filesystem::current_path().string();
    bp::BpModuleLinux bml;
    EXPECT_FALSE(bml.LoadModule(""));
    EXPECT_TRUE(bml.LoadModule(cur_path + "/../conf/bpmath.json"));

    auto pbmsg = bml.CreateModuleVal("bp.math.BpIntPair");
    EXPECT_TRUE(nullptr != pbmsg);
    auto pbintpair = std::static_pointer_cast<::bp_pb::BpIntPair>(pbmsg);
    pbintpair->set_a(100);

    auto func = bml.GetModuleFunc("add_int");
    auto p = std::make_shared<::bp_pb::BpIntPair>();
    p->set_a(1);
    p->set_b(1);
    auto res = std::any_cast<module_func2_t>(func.func)(p);
    EXPECT_EQ(std::static_pointer_cast<::bp_pb::BpInt>(res)->var(), 2);
}

TEST(bpcore, BpLib) {
    auto cur_path = std::filesystem::current_path().string();
    bp::BpLibLinux bll;
    EXPECT_TRUE(bll.Init(cur_path + "/../conf/"));
}