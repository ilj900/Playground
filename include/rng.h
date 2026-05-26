#pragma once

#include <random>

template <typename T>
class TRNG
{
    static_assert(std::is_arithmetic_v<T>);

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
    Generator(Seed),
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