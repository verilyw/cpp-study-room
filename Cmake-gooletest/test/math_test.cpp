#include "math.h"
#include <gtest/gtest.h>

TEST(MathAddTest, EasyAdditon) {
    EXPECT_EQ(2, math::add(-1, 3));
    EXPECT_EQ(0, math::add(-1, 1));
    EXPECT_EQ(3, math::add(1, 2));
}

TEST(MathSubTest, EasySubstraction) {
    EXPECT_EQ(1, math::sub(2, 1));
    EXPECT_EQ(-1, math::sub(1, 2));
}

TEST(MathMulTest, EasyMultiplication) {
    EXPECT_EQ(2, math::mul(1, 2));
    EXPECT_EQ(2, math::mul(2, 1));
    EXPECT_EQ(-2, math::mul(-2, 1));
    EXPECT_EQ(-2, math::mul(2, -1));
    EXPECT_EQ(0, math::mul(0, 100));
}