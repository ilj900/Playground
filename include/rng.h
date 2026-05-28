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
using DefaultEngineFor = std::conditional_t<
    (sizeof(T) > 4),
    std::mt19937_64,
    std::mt19937>;

template <typename T, typename TDistribution>
class TRNG
{
    static_assert(is_valid_rng_type_v<T>,
        "TRNG: type not supported by std::uniform distribution");

public:
    using ValueType = T;
    using EngineType = DefaultEngineFor<T>;
    using DistributionType = TDistribution;

    template <typename... Args>
    TRNG(uint64_t Seed, Args&&...DistributionArgs) :
        Generator(static_cast<EngineType::result_type>(Seed)),
        Distribution(std::forward<Args>(DistributionArgs)...)
    {}

    T operator()()
    {
        return Distribution(Generator);
    }
private:
    EngineType Generator;
    DistributionType Distribution;
};

class RNG
{
public:
    RNG() = delete;

    static auto GetDeterministicUniformFloatRNG(float Min, float Max, uint64_t Seed = 0)
    {
        return TRNG<float, std::uniform_real_distribution<float>>(Seed, Min, Max);
    }

    static auto GetDeterministicUniformDoubleRNG(double Min, double Max, uint64_t Seed = 0)
    {
        return TRNG<double, std::uniform_real_distribution<double>>(Seed, Min, Max);
    }

    static auto GetDeterministicUniformIntRNG(int Min, int Max, uint64_t Seed = 0)
    {
        return TRNG<int, std::uniform_int_distribution<int>>(Seed, Min, Max);
    }

    static auto GetDeterministicNormalFloatRNG(float Mean, float Std, uint64_t Seed = 0)
    {
        return TRNG<float, std::normal_distribution<float>>(Seed, Mean, Std);
    }

    static auto GetUniformFloatRNG(float Min, float Max)
    {
        return TRNG<float, std::uniform_real_distribution<float>>(std::random_device{}(), Min, Max);
    }

    static auto GetUniformDoubleRNG(double Min, double Max)
    {
        return TRNG<double, std::uniform_real_distribution<double>>(std::random_device{}(), Min, Max);
    }

    static auto GetUniformIntRNG(int Min, int Max)
    {
        return TRNG<int, std::uniform_int_distribution<int>>(std::random_device{}(), Min, Max);
    }
};