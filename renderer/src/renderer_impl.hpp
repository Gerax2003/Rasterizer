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
    float u;
    float v;
    float3 normals;
    float4 color;
    float4 worldPos;
};

struct Light
{
    bool enabled;
    float4 position; // world pos
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 attenuation;
};

struct Material
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
    float4 emissionColor;
    float shininess;
};

struct Uniforms
{
    mat4x4 modelViewProj;
    mat4x4 viewProj;
    mat4x4 model;
    mat4x4 view;
    mat4x4 proj;

    Texture texture;
    Light lights[8];
    Material material;
    
    float time;
    float deltaTime;

    bool gouraud = true;
};

struct rdrImpl
{
    Framebuffer fb;
    Viewport viewport;

    float4 lineColor = { 1.f, 1.f, 1.f, 1.f };

    Uniforms uniforms;
};
