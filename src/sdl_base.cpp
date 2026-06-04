#include "sdl_base.h"

void SDLInteractiveTest::Init(const std::string &Title, uint32_t Width, uint32_t Height)
{
    this->Width = Width;
    this->Height = Height;

    Image = ImageWrapperFactory::CreateRGB8(Width, Height);

    ASSERT_TRUE(SDL_Init(SDL_INIT_VIDEO)) << SDL_GetError();

    Window = SDL_CreateWindow(Title.c_str(), Width, Height, 0);
    ASSERT_NE(Window, nullptr) << SDL_GetError();

    WindowSurface = SDL_GetWindowSurface(Window);
    ASSERT_NE(WindowSurface, nullptr) << SDL_GetError();

    ImageSurface = SDL_CreateSurfaceFrom(
        Width,
        Height,
        SDL_PIXELFORMAT_RGB24,
        Image->Data.data(),
        Width * sizeof(RGB8)
    );
    ASSERT_NE(ImageSurface, nullptr) << SDL_GetError();

    Frame = 0;
}

void SDLInteractiveTest::Execute(std::function<void()> Update, std::function<void(const SDL_Event& Event)> ParseEvent)
{
    bRunning = true;

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

void SDLInteractiveTest::Finish()
{
    if (ImageSurface)
    {
        SDL_DestroySurface(ImageSurface);
        ImageSurface = nullptr;
    }

    if (Window)
    {
        SDL_DestroyWindow(Window);
        Window = nullptr;
    }

    WindowSurface = nullptr;

    if (SDL_WasInit(SDL_INIT_VIDEO))
    {
        SDL_Quit();
    }

    Image = nullptr;
}

void SDLInteractiveTest::TearDown()
{
    Finish();
}
