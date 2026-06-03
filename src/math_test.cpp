#include <gtest/gtest.h>
#include <SDL3/SDL.h>

#include "vec_math.h"
#include "timer.h"
#include "sdl_base.h"

TEST_F(SDLBase, Mandelbrot)
{
    Init("Mandelbrot", 2048, 2048);

    FPoint3 LeftTop = {-2.f, -2.f, 0.f};
    FVec3 Size = {4.f, 4.f, 0.f};
    bool bNeedsRedraw = true;

    auto Update = [&]()
    {
        Timer T("Mandelbrot frame time: ");

        if (bNeedsRedraw)
        {
            for (int py = 0; py < Height; py++)
            {
                for (int px = 0; px < Width; px++)
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

    Finish();
}
