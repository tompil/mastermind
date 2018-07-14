#include "../include/mastermind_utils.hpp"
#include "gmock/gmock.h"


TEST(MastermindUtilsTest, ShouldAreAllValuesDifferentReturnTrueForDistinctValues) {
    EXPECT_TRUE(mastermind::are_all_values_different<int>({ 1, 2, 3, 4 }));
}

TEST(MastermindUtilsTest, ShouldAreAllValuesDifferentReturnFalseForIndistinctValues) {
    EXPECT_FALSE(mastermind::are_all_values_different<int>({ 1, 2, 3, 3 }));
}
