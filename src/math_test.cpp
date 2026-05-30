#include <gtest/gtest.h>

#include "vec_math.h"
#include "img_wrapper.h"

TEST(Math, Mandelbrot)
{
    const int ImageWidth = 2048;
    auto Image = ImageWrapperFactory::CreateRGB8(ImageWidth, ImageWidth);

    FPoint3 LeftTop = {-0.732f, -0.292f, 0.f};
    FVec3 Size = {0.04f, 0.04f, 0.f};
    const int MaxIterations = 512;

    for (int py = 0; py < ImageWidth; py++)
    {
        for (int px = 0; px < ImageWidth; px++)
        {
            FVec3 C = {LeftTop.x + (float(px) / ImageWidth) * Size.x, LeftTop.y - (float(py) / ImageWidth) * Size.y, 0};
            FVec3 Z{};
            int Iter = 0;
            while (Iter < MaxIterations && Z.LengthSquared() < 4)
            {
                Z = {Z.x * Z.x - Z.y * Z.y + C.x, 2.f * Z.x * Z.y + C.y, 0.f};
                Iter++;
            }

            if (Iter == MaxIterations)
            {
                (*Image)[uint32_t(px), uint32_t(py)] = {0, 0, 0};
            }
            else
            {
                float t = std::sqrt(float(Iter) / MaxIterations);
                uint8_t v = uint8_t(t * 255.f);
                (*Image)[uint32_t(px), uint32_t(py)] = {v, v, v};
            }
        }
    }

    Image->Write("Mandelbrot.png");
}