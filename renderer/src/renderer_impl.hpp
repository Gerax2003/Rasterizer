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

struct Texture
{
    float* texture = nullptr;
    int texWidth;
    int texHeight;
};

struct Varying
{
    float4 light;
    float u;
    float v;
    float3 normals;
    float4 color;
};

struct Light
{
    float4 position;
    float4 color;
    bool enabled = false;
    float power;
};

struct Uniforms
{
    mat4x4 modelViewProj;
    mat4x4 model;
    mat4x4 view;
    mat4x4 proj;

    Texture texture;
    float time;
    float deltaTime;
    Light lights[8];
};

struct rdrImpl
{
    Framebuffer fb;
    Viewport viewport;

    float4 lineColor = { 1.f, 1.f, 1.f, 1.f };

    Uniforms uniforms;
};
