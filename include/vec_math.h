#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>

template <typename T>
struct Tuple3
{
    Tuple3() = default;
    Tuple3(T x, T y, T z) : x(x), y(y), z(z) {}

    T operator[](int i) const
    {
        assert(i >= 0 && i < 3);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    T& operator[](int i)
    {
        assert(i >= 0 && i < 3);
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    Tuple3 operator+(const Tuple3& other) const
    {
        return Tuple3(x + other.x, y + other.y, z + other.z);
    }

    Tuple3 operator-(const Tuple3& other) const
    {
        return Tuple3(x - other.x, y - other.y, z - other.z);
    }

    Tuple3 operator-() const
    {
        return Tuple3(-x, -y, -z);
    }

    Tuple3 operator*(T Value) const
    {
        return Tuple3(x * Value, y * Value, z * Value);
    }

    friend Tuple3 operator*(T Value, const Tuple3& d)
    {
        return d * Value;
    }

    Tuple3 operator/(T Value) const
    {
        assert(Value != T(0));
        return Tuple3(x / Value, y / Value, z / Value);
    }

    Tuple3& operator+=(const Tuple3& Other)
    {
        x += Other.x;
        y += Other.y;
        z += Other.z;
        return *this;
    }

    Tuple3& operator-=(const Tuple3& Other)
    {
        x -= Other.x;
        y -= Other.y;
        z -= Other.z;
        return *this;
    }

    Tuple3& operator*=(T Value)
    {
        x *= Value;
        y *= Value;
        z *= Value;
        return *this;
    }

    Tuple3& operator/=(T Value)
    {
        assert(Value != T(0));
        x /= Value;
        y /= Value;
        z /= Value;
        return *this;
    }

    Tuple3 operator*(const Tuple3& Other) const
    {
        return Tuple3(x * Other.x, y * Other.y, z * Other.z);
    }

    Tuple3& operator*=(const Tuple3& Other)
    {
        x *= Other.x;
        y *= Other.y;
        z *= Other.z;
        return *this;
    }

    bool operator==(const Tuple3& Other) const
    {
        if constexpr (std::is_integral_v<T>)
        {
            return x == Other.x && y == Other.y && z == Other.z;
        }
        else
        {
            return std::abs(x - Other.x) < kEpsilon && std::abs(y - Other.y) < kEpsilon && std::abs(z - Other.z) < kEpsilon;
        }
    }

    T Length() const
    {
        static_assert(std::is_floating_point_v<T>, "Length requires floating-point type");
        return static_cast<T>(std::sqrt(x * x + y * y + z * z));
    }

    T LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    Tuple3 GetNormalized() const
    {
        static_assert(std::is_floating_point_v<T>, "GetNormalized requires floating-point type");
        T Len = Length();
        assert(Len > kEpsilon);
        return (*this) / Len;
    }

    Tuple3& Normalize()
    {
        static_assert(std::is_floating_point_v<T>, "Normalize requires floating-point type");
        T Len = Length();
        assert(Len > kEpsilon);
        *this /= Len;
        return *this;
    }

    static constexpr T kEpsilon = static_cast<T>(1e-6);

    T x, y, z;
};

template <typename T>
Tuple3<T> cross(const Tuple3<T>& A, const Tuple3<T>& B)
{
    static_assert(std::is_floating_point_v<T>, "Cross requires floating-point type");
    return Tuple3<T>(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x);
}

template <typename T>
T dot(const Tuple3<T>& A, const Tuple3<T>& B)
{
    return A.x * B.x + A.y * B.y + A.z * B.z;
}

template <typename T>
Tuple3<T> normalize(const Tuple3<T>& A)
{
    static_assert(std::is_floating_point_v<T>, "normalize requires floating-point type");
    T Len = A.Length();
    assert(Len > Tuple3<T>::kEpsilon);
    return A / Len;
}

template <typename T>
Tuple3<T> abs(const Tuple3<T>& A)
{
    return Tuple3<T>(std::abs(A.x), std::abs(A.y), std::abs(A.z));
}

template <typename T>
Tuple3<T> ceil(const Tuple3<T>& A)
{
    return Tuple3<T>(std::ceil(A.x), std::ceil(A.y), std::ceil(A.z));
}

template <typename T>
Tuple3<T> floor(const Tuple3<T>& A)
{
    return Tuple3<T>(std::floor(A.x), std::floor(A.y), std::floor(A.z));
}

template <typename T>
Tuple3<T> lerp(float t, const Tuple3<T>& A, const Tuple3<T>& B)
{
    return (1 - t) * A + t * B;
}

template <typename T>
Tuple3<T> fma(const Tuple3<T>& A, const Tuple3<T>& B, const Tuple3<T>& C)
{
    return A * B + C;
}

template <typename T>
Tuple3<T> min(const Tuple3<T>& A, const Tuple3<T>& B)
{
    return Tuple3<T>(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z));
}

template <typename T>
Tuple3<T> max(const Tuple3<T>& A, const Tuple3<T>& B)
{
    return Tuple3<T>(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z));
}


using FPoint3 = Tuple3<float>;
using DPoint3 = Tuple3<double>;
using IPoint3 = Tuple3<int>;
using FVec3 = Tuple3<float>;
using DVec3 = Tuple3<double>;
using IVec3 = Tuple3<int>;