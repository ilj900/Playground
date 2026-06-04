#pragma once

#include "stb_image.h"
#include "stb_image_write.h"
#include "tinyexr.h"

#include <cassert>
#include <filesystem>
#include <memory>
#include <print>
#include <string>
#include <vector>

template <typename T, int C>
struct Pixel;

template <typename T>
struct Pixel<T, 1> {
    static constexpr int Channels = 3;
    T R;
};

template <typename T>
struct Pixel<T, 3> {
    static constexpr int Channels = 3;
    T R, G, B;
};

template <typename T>
struct Pixel<T, 4> {
    static constexpr int Channels = 4;
    T R, G, B, A;
};

using R8 = Pixel<uint8_t, 1>;
using RGB8 = Pixel<uint8_t, 3>;
using RGBA8 = Pixel<uint8_t, 4>;
using RGB32F = Pixel<float, 3>;
using RGBA32F = Pixel<float, 4>;

template <typename PixelT>
class ImageWrapper
{
    class PrivateTag {
        friend class ImageWrapperFactory;
        PrivateTag() = default;
    public:
        PrivateTag(const PrivateTag&) = delete;
        PrivateTag(PrivateTag&&) = default;
        PrivateTag& operator=(const PrivateTag&) = delete;
        PrivateTag& operator=(PrivateTag&&) = delete;
    };

    uint32_t Width = 0;
    uint32_t Height = 0;

    friend class ImageWrapperFactory;

public:
    ImageWrapper(PrivateTag, uint32_t Width, uint32_t Height) :
    Width(Width), Height(Height), Data(Width * Height)
    {}

    ImageWrapper() = delete;
    ImageWrapper(const ImageWrapper&) = delete;
    ImageWrapper& operator=(const ImageWrapper&) = delete;

    ImageWrapper(ImageWrapper&&) = default;
    ImageWrapper& operator=(ImageWrapper&&) = default;

    PixelT& operator[](uint32_t x, uint32_t y)
    {
        assert(x < Width && y < Height && "Image index out of bounds");
        return Data[y * Width + x];
    }

    const PixelT& operator[](uint32_t x, uint32_t y) const
    {
        assert(x < Width && y < Height && "Image index out of bounds");
        return Data[y * Width + x];
    }

    void Write(const std::string& Path) const
    {
        using T = decltype(PixelT::R);
        int W = static_cast<int>(Width);
        int H = static_cast<int>(Height);

        if constexpr (std::is_same_v<T, float>)
        {
            const char* err = nullptr;
            int Result = SaveEXR(reinterpret_cast<const float*>(Data.data()), W, H, PixelT::Channels, 0, Path.c_str(), &err);
            if (Result != TINYEXR_SUCCESS) {
                if (err) {
                    std::println("Failed to save EXR: {}", err);
                    FreeEXRErrorMessage(err);
                }
                throw std::runtime_error("Failed to save EXR image");
            }

            return;
        }

        if constexpr (std::is_same_v<T, uint8_t>)
        {
            auto Extension = std::filesystem::path(Path).extension().string();
            std::transform(Extension.begin(), Extension.end(), Extension.begin(), ::tolower);
            int Result = 0;

            auto* raw = reinterpret_cast<const unsigned char*>(Data.data());

            if (Extension == ".png")
                Result = stbi_write_png(Path.c_str(), W, H, PixelT::Channels, raw, W * PixelT::Channels);
            else if (Extension == ".jpg" || Extension == ".jpeg")
                Result = stbi_write_jpg(Path.c_str(), W, H, PixelT::Channels, raw, 90);
            else if (Extension == ".bmp")
                Result = stbi_write_bmp(Path.c_str(), W, H, PixelT::Channels, raw);
            else if (Extension == ".tga")
                Result = stbi_write_tga(Path.c_str(), W, H, PixelT::Channels, raw);
            else throw std::runtime_error("Unsupported output format: " + Extension);

            if (!Result)
                throw std::runtime_error("Failed to save image: " + Path);

            return;
        }

        throw std::runtime_error("Not implemented");
    }

    auto begin()       { return Data.begin(); }
    auto end()         { return Data.end(); }
    auto begin() const { return Data.begin(); }
    auto end()   const { return Data.end(); }

    std::vector<PixelT> Data;
};

class ImageWrapperFactory
{
public:
    template <typename T, int C>
    static std::shared_ptr<ImageWrapper<Pixel<T, C>>> Create(uint32_t Width, uint32_t Height)
    {
        using ImageT = ImageWrapper<Pixel<T, C>>;
        return std::make_shared<ImageT>(typename ImageT::PrivateTag{}, Width, Height);
    }

    static auto CreateR8   (uint32_t Width, uint32_t Height)
    {
        return Create<uint8_t, 1>(Width, Height);
    }
    static auto CreateRGB8   (uint32_t Width, uint32_t Height)
    {
        return Create<uint8_t, 3>(Width, Height);
    }
    static auto CreateRGBA8  (uint32_t Width, uint32_t Height)
    {
        return Create<uint8_t, 4>(Width, Height);
    }
    static auto CreateRGB32F (uint32_t Width, uint32_t Height)
    {
        return Create<float,   3>(Width, Height);
    }
    static auto CreateRGBA32F(uint32_t Width, uint32_t Height)
    {
        return Create<float,   4>(Width, Height);
    }

    template <int C>
    static std::shared_ptr<ImageWrapper<Pixel<float, C>>> OpenFloat(const std::string& Path)
    {
        int Width, Height;
        float* Data = nullptr;
        const char* err = nullptr;

        auto Result = LoadEXR(&Data, &Width, &Height, Path.c_str(), &err);
        std::unique_ptr<float[], decltype(&free)> DataGuard(Data, free);

        if (Result != TINYEXR_SUCCESS) {
            if (err) {
                std::println("Failed to load image: {}", err);
                FreeEXRErrorMessage(err);
            }
            throw std::runtime_error("Failed to load EXR image");
        }

        auto Image = Create<float, C>(Width, Height);

        for (uint32_t p = 0; p < uint32_t(Width) * uint32_t(Height); ++p)
        {
            const float* Src = Data + p * 4; // EXR always 4 floats/pixel
            auto& Dst = Image->Data[p];

            Dst.R = Src[0];
            if constexpr (C >= 3) { Dst.G = Src[1]; Dst.B = Src[2]; }
            if constexpr (C >= 4) Dst.A = Src[3];
        }

        return Image;
    }

    template <int C>
    static std::shared_ptr<ImageWrapper<Pixel<uint8_t, C>>> OpenUINT8(const std::string& Path)
    {
        /// Try opening with stb
        int Width, Height, Channels;
        unsigned char *Data = stbi_load(Path.c_str(), &Width, &Height, &Channels, C);

        if (!Data) {
            std::println("Failed to load image: {}", stbi_failure_reason());
            throw std::runtime_error("Failed to load image");
        }

        std::unique_ptr<unsigned char, decltype(&stbi_image_free)> DataGuard(Data, stbi_image_free);

        auto Image = Create<uint8_t, C>(Width, Height);

        for (uint32_t p = 0; p < uint32_t(Width) * uint32_t(Height); ++p) {
            const unsigned char* Src = Data + p * C;
            auto& Dst = Image->Data[p];

            Dst.R = Src[0];
            if constexpr (C >= 3) { Dst.G = Src[1]; Dst.B = Src[2]; }
            if constexpr (C >= 4) Dst.A = Src[3];
        }

        return Image;
    }

    static auto OpenRGB8   (const std::string& p)
    {
        return OpenUINT8<3>(p);
    }
    static auto OpenRGBA8  (const std::string& p)
    {
        return OpenUINT8<4>(p);
    }
    static auto OpenRGB32F (const std::string& p)
    {
        return OpenFloat<3>(p);
    }
    static auto OpenRGBA32F(const std::string& p)
    {
        return OpenFloat<4>(p);
    }
};

inline RGB8 HashColor(uint32_t Seed) noexcept
{
    Seed ^= Seed >> 16;
    Seed *= 0x85ebca6bu;
    Seed ^= Seed >> 13;
    Seed *= 0xc2b2ae35u;
    Seed ^= Seed >> 16;

    return {
        .R = static_cast<uint8_t>(Seed >> 16),
        .G = static_cast<uint8_t>(Seed >> 8),
        .B = static_cast<uint8_t>(Seed),
    };
}

