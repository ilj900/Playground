#include "rng.h"
#include <gtest/gtest.h>
#include <print>

TEST(Random, Basic)
{
    auto RNG = RNG::GetDeterministicUniformFloatRNG(0, 1);

    for (int i = 0; i < 20; i++)
    {
        std::println("{}", RNG());
    }
}