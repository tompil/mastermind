#include "../include/mastermind_engine.hpp"
#include "gtest/gtest.h"

TEST(MastermindEngineTest, IsCreatable) {
    mastermind_engine me;
    ASSERT_TRUE(true);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
