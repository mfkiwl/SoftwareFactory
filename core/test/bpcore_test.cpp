#include <filesystem>
#include <gtest/gtest.h>

#include "../BpContents.hpp"
#include "../BpModule.hpp"
#include "../BpModuleLinux.hpp"
#include "../BpLibLinux.hpp"

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
    std::cout << bp2->GetFullPath() << std::endl;
    bp3->SetParent(bp1);
    bp4->SetParent(bp3);
    EXPECT_EQ(bp4->GetFullPath(), "bp.math.pb_int_a");
    std::cout << bp4->GetFullPath() << std::endl;
}

TEST(bpcore, BpModule) {
    auto cur_path = std::filesystem::current_path().string();
    bp::BpModuleLinux bml;
    EXPECT_FALSE(bml.LoadModule(""));
    EXPECT_TRUE(bml.LoadModule(cur_path + "/../conf/bpmath.json"));
}

TEST(bpcore, BpLib) {
    auto cur_path = std::filesystem::current_path().string();
    bp::BpLibLinux bll;
    EXPECT_TRUE(bll.Init(cur_path + "/../conf/"));
}