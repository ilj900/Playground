#pragma once

#include <gtest/gtest.h>

#include "SDL3/SDL.h"

#include "img_wrapper.h"

class SDLBase : public ::testing::Test
{
public:
    void Init(const std::string& Title, int Width, int Height);
    void Finish();
    void Execute(std::function<void()> Update, std::function<void(const SDL_Event& Event)> ParseEvent = {});

protected:
    uint32_t Width = 0;
    uint32_t Height = 0;
    SDL_Window* Window = nullptr;
    SDL_Surface* WindowSurface = nullptr;
    SDL_Surface* ImageSurface = nullptr;
    std::shared_ptr<ImageWrapper<Pixel<unsigned char, 3>>> Image = nullptr;

private:
    bool bRunning = false;
};
