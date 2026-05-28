#pragma once

#include <random>

template <typename T>
inline constexpr bool is_valid_rng_type_v =
    std::is_same_v<T, short>              ||
    std::is_same_v<T, int>                ||
    std::is_same_v<T, long>               ||
    std::is_same_v<T, long long>          ||
    std::is_same_v<T, unsigned short>     ||
    std::is_same_v<T, unsigned int>       ||
    std::is_same_v<T, unsigned long>      ||
    std::is_same_v<T, unsigned long long> ||
    std::is_same_v<T, float>              ||
    std::is_same_v<T, double>             ||
    std::is_same_v<T, long double>;

template <typename T>
class TRNG
{
    static_assert(is_valid_rng_type_v<T>,
        "TRNG: type not supported by std::uniform distribution");

    using EngineType = std::conditional_t<
          (sizeof(T) > 4),
          std::mt19937_64,
          std::mt19937>;


    using DistType = std::conditional_t<
        std::is_integral_v<T>,
        std::uniform_int_distribution<T>,
        std::uniform_real_distribution<T>
        >;

public:
    TRNG(T Min, T Max, uint64_t Seed) :
    Generator(static_cast<EngineType::result_type>(Seed)),
    Distribution(Min, Max)  {}

    T operator()()
    {
        return Distribution(Generator);
    }
private:
    EngineType Generator;
    DistType Distribution;
};

class RNG
{
public:
    RNG() = delete;

    static auto GetDeterministicUniformFloatRNG(float Min, float Max, uint64_t Seed = 0)
    {
        return TRNG(Min, Max, Seed);
    }

    static auto GetDeterministicUniformDoubleRNG(double Min, double Max, uint64_t Seed = 0)
    {
        return TRNG(Min, Max, Seed);
    }

    static auto GetDeterministicUniformIntRNG(int Min, int Max, uint64_t Seed = 0)
    {
        return TRNG(Min, Max, Seed);
    }

    static auto GetUniformFloatRNG(float Min, float Max)
    {
        return TRNG(Min, Max, std::random_device{}());
    }

    static auto GetUniformDoubleRNG(double Min, double Max)
    {
        return TRNG(Min, Max, std::random_device{}());
    }

    static auto GetUniformIntRNG(int Min, int Max)
    {
        return TRNG(Min, Max, std::random_device{}());
    }
};