#include <gtest/gtest.h>

#include <print>

TEST(BasicTest, HelloWorld)
{
    std::println("Hello World!");
    EXPECT_EQ(1, 1);
}