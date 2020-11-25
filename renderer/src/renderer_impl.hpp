#pragma once

#include <rdr/renderer.h>

#include <common/types.hpp>

struct Viewport
{
    int x;
    int y;
    int width;
    int height;
};

struct Framebuffer
{
    int width;
    int height;
    float4* colorBuffer;
    float* depthBuffer;
};

struct rdrImpl
{
    Framebuffer fb;
    Viewport viewport;

    mat4x4 model;
    mat4x4 view;
    mat4x4 projection;

    float4 lineColor = { 1.f, 1.f, 1.f, 1.f };
};

struct Varying
{
    float light;
    float u;
    float v;
    float3 norms;
    float4 color;
};