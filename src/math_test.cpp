#include <gtest/gtest.h>
#include <SDL3/SDL.h>

#include "rng.h"
#include "vec_math.h"
#include "timer.h"
#include "sdl_base.h"

TEST_F(SDLInteractiveTest, Mandelbrot)
{
    ASSERT_NO_FATAL_FAILURE(Init("Mandelbrot", 2048, 2048));

    FPoint3 LeftTop = {-2.f, -2.f, 0.f};
    FVec3 Size = {4.f, 4.f, 0.f};
    bool bNeedsRedraw = true;

    auto Update = [&]()
    {
        if (bNeedsRedraw)
        {
            Timer T("Mandelbrot frame time: ");

            for (uint32_t py = 0; py < Height; py++)
            {
                for (uint32_t px = 0; px < Width; px++)
                {
                    FVec3 C = {LeftTop.x + (float(px) / Width) * Size.x, LeftTop.y + (float(py) / Height) * Size.y, 0};
                    FVec3 Z{};

                    constexpr int MaxIterations = 512;
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

            bNeedsRedraw = false;
        }
    };

    auto PoolEvents = [&](const SDL_Event& Event)
    {
        if (Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            float px = Event.button.x;
            float py = Event.button.y;
            float cx = LeftTop.x + (px / Width) * Size.x;
            float cy = LeftTop.y + (py / Height) * Size.y;
            float scale = 1.0f;
            if (Event.button.button == SDL_BUTTON_LEFT)
            {
                bNeedsRedraw = true;
                scale = 0.5f;
            }
            if (Event.button.button == SDL_BUTTON_RIGHT)
            {
                bNeedsRedraw = true;
                scale = 2.0f;
            }
            Size.x *= scale;
            Size.y *= scale;
            LeftTop.x = cx - (px / Width) * Size.x;
            LeftTop.y = cy - (py / Height) * Size.y;
        }
    };

    Execute(Update, PoolEvents);
}

TEST_F(SDLInteractiveTest, GameOfLife)
{
    ASSERT_NO_FATAL_FAILURE(Init("GameOfLife", 2048, 2048));

    auto ImgA = ImageWrapperFactory::CreateR8(Width, Height);
    auto ImgB = ImageWrapperFactory::CreateR8(Width, Height);

    auto RNG = RNG::GetDeterministicUniformIntRNG(0, 1);

    for (uint32_t py = 0; py < Height; py++)
    {
        for (uint32_t px = 0; px < Width; px++)
        {
            (*ImgA)[px, py].R = RNG();
        }
    }

    auto Update = [&]()
    {
        Timer T("GameOfLife frame time: ");

        auto Image1 = Frame % 2 == 0 ? ImgA : ImgB;
        auto Image2 = Frame % 2 == 0 ? ImgB : ImgA;

        for (uint32_t py = 0; py < Height; py++)
        {
            uint32_t u = (py + Height - 1) % Height;
            uint32_t d = (py + 1) % Height;

            for (uint32_t px = 0; px < Width; px++)
            {
                uint32_t l = (px + Width - 1) % Width;
                uint32_t r = (px + 1) % Width;

                uint32_t copy = (*Image1)[px, py].R;
                uint32_t c = (*Image1)[l, u].R;
                c += (*Image1)[px, u].R;
                c += (*Image1)[r, u].R;
                c += (*Image1)[l, py].R;
                c += (*Image1)[r, py].R;
                c += (*Image1)[l, d].R;
                c += (*Image1)[px, d].R;
                c += (*Image1)[r, d].R;

                c = c == 2 ? copy : c == 3 ? 1 : 0;

                (*Image2)[px, py] = {static_cast<uint8_t>(c)};
                (*Image)[px, py] = {static_cast<uint8_t>(c * 255u), 0, 0};
            }
        }

    };

    Execute(Update);
}

TEST_F(SDLInteractiveTest, Voronoi)
{
    ASSERT_NO_FATAL_FAILURE(Init("Voronoi", 2048, 2048));

    auto RNGW = RNG::GetDeterministicUniformFloatRNG(0, float(Width), 0);
    auto RNGH = RNG::GetDeterministicUniformFloatRNG(0, float(Height), 1);
    std::vector<FPoint3> Points(50);
    for (auto& P : Points)
    {
        P.x = RNGW();
        P.y = RNGH();
    }

    auto BackImage = ImageWrapperFactory::CreateRGB8(Width, Height);

    bool bNeedsRedraw = true;

    auto Update = [&]()
    {
        if (!bNeedsRedraw)
        {
            return;
        }

        Timer T("Voronoi frame time: ");

        for (uint32_t py = 0; py < Height; py++)
        {
            for (uint32_t px = 0; px < Width; px++)
            {
                float BestDist2 = std::numeric_limits<float>::max();
                float SecondBestDist2 = std::numeric_limits<float>::max();
                uint32_t BestIndex = 0;

                FPoint3 P = {float(px), float(py), 0};

                for (uint32_t i = 0; i < Points.size(); i++)
                {
                    auto D = P - Points[i];
                    auto Dist2 = D.LengthSquared();

                    if (Dist2 < BestDist2)
                    {
                        SecondBestDist2 = BestDist2;
                        BestDist2 = Dist2;
                        BestIndex = i;
                    }
                    else if (Dist2 < SecondBestDist2)
                    {
                        SecondBestDist2 = Dist2;
                    }
                }

                (*BackImage)[px, py] = HashColor(BestIndex + 1);  /// +1 because 0 is black
            }
        }

        bNeedsRedraw = false;

        // AA, but only a central part of the image, borders are ignored (Who will check it?)
        for (uint32_t py = 1; py < Height - 1; py++)
        {
            for (uint32_t px = 1; px < Width - 1; px++)
            {
                auto Converter = [](const RGB8& P) -> FPoint3
                {
                    FPoint3 Result{};
                    Result.x = float(P.R) / 255.0f;
                    Result.y = float(P.G) / 255.0f;
                    Result.z = float(P.B) / 255.0f;
                    return Result;
                };

                FPoint3 Mix = Converter((*BackImage)[px, py]);
                Mix += Converter((*BackImage)[px + 1, py]);
                Mix += Converter((*BackImage)[px - 1, py]);
                Mix += Converter((*BackImage)[px, py + 1]);
                Mix += Converter((*BackImage)[px, py - 1]);
                Mix += Converter((*BackImage)[px + 1, py + 1]);
                Mix += Converter((*BackImage)[px + 1, py - 1]);
                Mix += Converter((*BackImage)[px - 1, py + 1]);
                Mix += Converter((*BackImage)[px - 1, py - 1]);

                Mix /= 9;

                (*Image)[px, py] = {static_cast<uint8_t>(Mix.x * 255), static_cast<uint8_t>(Mix.y * 255), static_cast<uint8_t>(Mix.z * 255)};
            }
        }

    };

    Execute(Update);
}