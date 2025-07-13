#include <gtest/gtest.h>

// Simple test that always passes
TEST(SampleTest, AlwaysPasses) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

// Another simple test
TEST(SampleTest, BasicMath) {
    EXPECT_EQ(2 + 2, 4);
    EXPECT_NE(3 + 3, 7);
}