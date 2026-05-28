#include "img_wrapper.h"
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

TEST(Random, RandomImage)
{
    uint32_t ImageWidth = 1024;
    auto Image = ImageWrapperFactory::CreateRGB8(ImageWidth, ImageWidth);

    auto RNG = RNG::GetDeterministicUniformFloatRNG(0, 1);
    for (uint32_t i = 0; i < 25 * ImageWidth * ImageWidth; i++)
    {
        float X = RNG();
        float Y = RNG();
        uint32_t X_i = uint32_t(X * ImageWidth);
        uint32_t Y_i = uint32_t(Y * ImageWidth);
        (*Image)[X_i, Y_i].R = static_cast<uint8_t>(std::min((*Image)[X_i, Y_i].R + 1u, 255u));
    }

    Image->Write("test.png");
}