#include "sdl_base.h"

#include <print>

void SDLBase::Init(const std::string &Title, int Width, int Height)
{
    this->Width = Width;
    this->Height = Height;

    Image = ImageWrapperFactory::CreateRGB8(Width, Height);

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::println("SDL_Init failed: {}", SDL_GetError());
        FAIL();
    }

    Window = SDL_CreateWindow(Title.c_str(), Width, Height, 0);

    if (!Window)
    {
        std::println("SDL_CreateWindow failed: {}", SDL_GetError());
        SDL_Quit();
        FAIL();
    }

    WindowSurface = SDL_GetWindowSurface(Window);

    ImageSurface = SDL_CreateSurfaceFrom(
        Width,
        Height,
        SDL_PIXELFORMAT_RGB24,
        Image->Data.data(),
        Width * sizeof(RGB8)
    );
}

void SDLBase::Execute(std::function<void()> Update, std::function<void(const SDL_Event& Event)> ParseEvent)
{
    bRunning = true;
    int Frame = 0;

    while (bRunning)
    {
        SDL_Event Event;

        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_EVENT_QUIT)
            {
                bRunning = false;
            }
            if (ParseEvent)
            {
                ParseEvent(Event);
            }
        }

        Update();

        SDL_BlitSurface(ImageSurface, nullptr, WindowSurface, nullptr);
        SDL_UpdateWindowSurface(Window);

        ++Frame;
        SDL_Delay(16);
    }
}

void SDLBase::Finish()
{
    SDL_DestroySurface(ImageSurface);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}
