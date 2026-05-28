#include <gtest/gtest.h>

#include <print>

TEST(BasicTest, HelloWorld)
{
    std::println("Hello World!");
    EXPECT_EQ(1, 1);
}

TEST(BasicTest, FloatCounter)
{
    uint32_t Counter = 0;
    uint32_t i = 0;
    float Val = 0.f;

    while (true)
    {
        Val = std::bit_cast<float>(i);

        if (Val >= 0.f && Val < 1.f)
        {
            Counter++;
        }

        if (i == std::numeric_limits<uint32_t>::max()) {
            break;
        }

        ++i;
    }

    std::println("Total number of floats in [0, 1) range: {}", Counter);
    std::println("It's {:.4f}% of all possible float values",
        static_cast<double>(Counter) / static_cast<double>(std::numeric_limits<uint32_t>::max()) * 100.0);
}