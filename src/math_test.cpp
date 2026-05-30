#include <gtest/gtest.h>
#include <SDL3/SDL.h>

#include "vec_math.h"
#include "img_wrapper.h"

TEST(Math, Mandelbrot)
{
    const int ImageWidth = 2048;
    auto Image = ImageWrapperFactory::CreateRGB8(ImageWidth, ImageWidth);

    FPoint3 LeftTop = {-0.732f, -0.292f, 0.f};
    FVec3 Size = {0.04f, 0.04f, 0.f};
    const int MaxIterations = 512;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::println("SDL_Init failed: {}", SDL_GetError());
        FAIL();
    }

    SDL_Window* Window = SDL_CreateWindow("CPU Framebuffer", ImageWidth, ImageWidth, 0);

    if (!Window)
    {
        std::println("SDL_CreateWindow failed: {}", SDL_GetError());
        SDL_Quit();
        FAIL();
    }

    SDL_Surface* WindowSurface = SDL_GetWindowSurface(Window);

    SDL_Surface* ImageSurface = SDL_CreateSurfaceFrom(
        ImageWidth,
        ImageWidth,
        SDL_PIXELFORMAT_RGB24,
        Image->Data.data(),
        ImageWidth * sizeof(RGB8)
    );


    bool Running = true;
    int Frame = 0;

    while (Running)
    {
        SDL_Event Event;

        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_EVENT_QUIT)
                Running = false;
        }

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

        SDL_BlitSurface(ImageSurface, nullptr, WindowSurface, nullptr);
        SDL_UpdateWindowSurface(Window);

        ++Frame;
        SDL_Delay(16);
    }

    SDL_DestroySurface(ImageSurface);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}