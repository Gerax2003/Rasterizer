#pragma once

#include <cmath>

#include "types.hpp"

// Constant and common maths functions
namespace maths
{
    const float TAU = 6.283185307179586476925f;

    inline float cos(float x) { return cosf(x); }
    inline float sin(float x) { return sinf(x); }
    inline float tan(float x) { return tanf(x); }
}

namespace mat4
{
    inline mat4x4 identity()
    {
        return {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }

    mat4x4 frustum(float left, float right, float bottom, float top, float near, float far);
    mat4x4 perspective(float fovY, float aspect, float near, float far);
    mat4x4 rotateX(float angleRadians);
    mat4x4 rotateY(float angleRadians);
    mat4x4 rotateZ(float angleRadians);
    mat4x4 scale(float scale);
    mat4x4 translate(float3 translate);
}

namespace v3
{
    float3 cross(float3 vec1, float3 vec2);

    inline float lengthVector3(float3 vector) { return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z); };
    inline float squaredLengthV3(float3 vector) { return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z; };
    inline float dotProductVector3(float3 vector1, float3 vector2) { return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z; };

    float3 unitVector3(float3 vector);
}

inline float4 operator*(const mat4x4& m, float4 v)
{
    float4 retFloat = {};

    for (int i = 0; i < 4; i++)
        retFloat.e[i] = v.e[0] * m.c[0].e[i] + v.e[1] * m.c[1].e[i] + v.e[2] * m.c[2].e[i] + v.e[3] * m.c[3].e[i];

    return retFloat;
};

inline mat4x4 operator*(const mat4x4& a, const mat4x4& b)
{
    mat4x4 retMat = {};

    for (int line = 0; line < 4; line++)
        for (int column = 0; column < 4; column++)
            for (int i = 0; i < 4; i++)
                retMat.c[column].e[line] += a.c[i].e[line] * b.c[column].e[i];

    return retMat;
};

inline float3 operator/(float3 v, float a) { return { v.x / a, v.y / a, v.z / a }; };
inline float3 operator-(float3 v) { return { -v.x, -v.y, -v.z }; };
inline float3 operator-(float3 v1, float3 v2) { return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z }; };
inline float3 operator*(float3 v1, float3 v2) { return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z }; };
inline float3 operator+(float3 v1, float3 v2) { return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z }; };
inline float3 operator*(float3 v, float a) { return { v.x * a, v.y * a, v.z * a }; };

inline float4 operator*(float4 v, float a){ return { v.x * a, v.y * a, v.z * a, v.w * a }; };
inline float4 operator+(float4 a, float4 b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; };