#pragma once

#include "vec_math.h"

struct Ray
{
    Ray() = default;
    Ray(FPoint3 Origin, FVec3 Direction) : O(Origin), D(Direction) {}

    FPoint3 operator()(float T) const
    {
        return O + T * D;
    }

    FPoint3 O;
    FVec3 D;
};

struct AABB
{
    AABB()
    {
        float MinNum = std::numeric_limits<float>::lowest();
        float MaxNum = std::numeric_limits<float>::max();
        Min = FPoint3(MaxNum, MaxNum, MaxNum);
        Max = FPoint3(MinNum, MinNum, MinNum);
    };

    AABB(FPoint3 A, FPoint3 B)
    {
        Min = min(A, B);
        Max = max(A, B);
    }

    AABB(const AABB& A, const AABB& B) :
        Min(min(A.Min, B.Min)),
        Max(max(A.Max, B.Max))
    {}

    AABB Union(const FPoint3& P) const
    {
        AABB Result;
        Result.Min = min(Min, P);
        Result.Max = max(Max, P);
        return Result;
    }

    AABB Union(const AABB& Other) const
    {
        return AABB(*this, Other);
    }

    AABB& Encompass(const AABB& Other)
    {
        Min = min(Min, Other.Min);
        Max = max(Max, Other.Max);
        return *this;
    }

    AABB Intersect(const AABB& Box) const
    {
        AABB Result;
        Result.Min = max(Min, Box.Min);
        Result.Max = min(Max, Box.Max);
        return Result;
    }

    bool Overlaps(const AABB& Other) const
    {
        return !(Max.x < Other.Min.x || Max.y < Other.Min.y || Max.z < Other.Min.z ||
                 Min.x > Other.Max.x || Min.y > Other.Max.y || Min.z > Other.Max.z);
    }

    bool Inside(const FPoint3& P) const
    {
        return  P.x >= Min.x && P.x <= Max.x &&
                P.y >= Min.y && P.y <= Max.y &&
                P.z >= Min.z && P.z <= Max.z;
    }

    float DistanceSquared(const FPoint3& P) const
    {
        float DX = std::max(std::max(Min.x - P.x, P.x - Max.x), 0.0f);
        float DY = std::max(std::max(Min.y - P.y, P.y - Max.y), 0.0f);
        float DZ = std::max(std::max(Min.z - P.z, P.z - Max.z), 0.0f);

        return DX * DX + DY * DY + DZ * DZ;
    }

    float Distance(const FPoint3& P) const
    {
        return std::sqrt(DistanceSquared(P));
    }

    FVec3 Diagonal() const
    {
        return Max - Min;
    }

    FPoint3 Center() const
    {
        return (Min + Max) * 0.5f;
    }

    float SurfaceArea() const
    {
        FVec3 Diag = Diagonal();
        return 2.0f * (Diag.x * Diag.y + Diag.x * Diag.z + Diag.y * Diag.z);
    }

    float Volume() const
    {
        FVec3 Diag = Diagonal();
        return Diag.x * Diag.y * Diag.z;
    }

    FPoint3 Min, Max;
};