
#include <cstdio>
#include <cstring>
#include <cassert>

#include <imgui.h>

#include <common/maths.hpp>

#include "renderer_impl.hpp"

rdrImpl* rdrInit(float* colorBuffer32Bits, float* depthBuffer, int width, int height)
{
    rdrImpl* renderer = new rdrImpl();

    renderer->fb.colorBuffer = reinterpret_cast<float4*>(colorBuffer32Bits);
    renderer->fb.depthBuffer = depthBuffer;
    renderer->fb.width  = width;
    renderer->fb.height = height;

    renderer->viewport = Viewport{ 0, 0, width, height };

    return renderer;
}

void rdrShutdown(rdrImpl* renderer)
{
    delete renderer;
}

void rdrSetProjection(rdrImpl* renderer, float* projectionMatrix)
{
    memcpy(renderer->projection.e, projectionMatrix, 16 * sizeof(float));
}

void rdrSetView(rdrImpl* renderer, float* viewMatrix)
{
    memcpy(renderer->view.e, viewMatrix, 16 * sizeof(float));
}

void rdrSetModel(rdrImpl* renderer, float* modelMatrix)
{
    memcpy(renderer->model.e, modelMatrix, 16 * sizeof(float));
}

void rdrSetViewport(rdrImpl* renderer, int x, int y, int width, int height)
{
    Viewport viewport = { x, y, width, height };
    renderer->viewport = viewport;
}

void rdrSetTexture(rdrImpl* renderer, float* colors32Bits, int width, int height)
{
    // TODO
}



void drawPixel(float4* colorBuffer, int width, int height, int x, int y, float4 color)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    int bufferIndex = (x + (y * width));
    colorBuffer[bufferIndex] = color;
}

void drawLine(float4* colorBuffer, int width, int height, int x0, int y0, int x1, int y1, float4 color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;) {
        drawPixel(colorBuffer, width, height, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void drawLine(const Framebuffer& fb, float3 p0, float3 p1, float4 color)
{
    drawLine(fb.colorBuffer, fb.width, fb.height, (int)roundf(p0.x), (int)roundf(p0.y), (int)roundf(p1.x), (int)roundf(p1.y), color);
}

float3 getBarycentricCoordinates(float3* points, float3 point)
{
    float lambda0 = ((points[1].y - points[2].y) * (point.x - points[2].x) + (points[2].x - points[1].x) * (point.y - points[2].y)) / ((points[1].y - points[2].y) * (points[0].x - points[2].x) + (points[2].x - points[1].x) * (points[0].y - points[2].y));
    float lambda1 = ((points[2].y - points[0].y) * (point.x - points[2].x) + (points[0].x - points[2].x) * (point.y - points[2].y)) / ((points[1].y - points[2].y) * (points[0].x - points[2].x) + (points[2].x - points[1].x) * (points[0].y - points[2].y));
    float lambda2 = 1 - lambda0 - lambda1;

    return { lambda0, lambda1, lambda2 };
}

int getLeftPoint(float3* points)
{
    int retNum = INT_MAX;
    for (int i = 0; i < 3; i++)
        if ((int)points[i].x < retNum)
            retNum = (int)points[i].x;

    return retNum;
}

int getRightPoint(float3* points)
{
    int retNum = INT_MIN;
    for (int i = 0; i < 3; i++)
        if ((int)points[i].x > retNum)
            retNum = (int)points[i].x;

    return retNum;
}

int getTopPoint(float3* points)
{
    int retNum = INT_MIN;
    for (int i = 0; i < 3; i++)
        if ((int)points[i].y > retNum)
            retNum = (int)points[i].y;

    return retNum;
}

int getBotPoint(float3* points)
{
    int retNum = INT_MAX;
    for (int i = 0; i < 3; i++)
        if ((int)points[i].y < retNum)
            retNum = (int)points[i].y;

    return retNum;
}

float4 getBoundingBox(float3* points)
{
    int leftPoint = getLeftPoint(points);
    int rightPoint = getRightPoint(points);
    int topPoint = getTopPoint(points);
    int botPoint = getBotPoint(points);
    int width = rightPoint - leftPoint;
    int height = botPoint - topPoint;

    float4 retBox = { (float)leftPoint, (float)topPoint, (float)width, (float)height };
    return retBox;
}

float4 getColor(float4* colors, float3 baryCoords)
{
    float r = baryCoords.x * colors[0].r + baryCoords.y * colors[1].r + baryCoords.z * colors[2].r;
    float g = baryCoords.x * colors[0].g + baryCoords.y * colors[1].g + baryCoords.z * colors[2].g;
    float b = baryCoords.x * colors[0].b + baryCoords.y * colors[1].b + baryCoords.z * colors[2].b;
    float a = baryCoords.x * colors[0].a + baryCoords.y * colors[1].a + baryCoords.z * colors[2].a;

    return { r, g, b, a };
}

bool depthTest(float3 point, Framebuffer buffer)
{
    if (point.x < 0 || point.x >= buffer.width || point.y < 0 || point.y >= buffer.height)
        return;

    int index = point.x + (point.y * buffer.width);

    if (point.z > buffer.depthBuffer[index])
        return true;
    else
        return false;
}

void fillTriangle(const Framebuffer& fb, float3* points, float4* colors)
{
    float4 box = getBoundingBox(points);

    // box.e[2] = width, box.e[3] = height
    /* Bounding box display
    drawLine(fb, { box.x, box.y }, { box.x + box.e[2], box.y }, colors[0]);
    drawLine(fb, { box.x + box.e[2], box.y }, { box.x + box.e[2], box.y + box.e[3] }, colors[0]);
    drawLine(fb, { box.x + box.e[2], box.y + box.e[3] }, { box.x, box.y + box.e[3] }, colors[0]);
    drawLine(fb, { box.x, box.y + box.e[3] }, { box.x, box.y }, colors[0]);*/
    
    for (int x = (int)box.x; x <= (int)box.x + (int)box.e[2]; x++)
    {
        for (int y = (int)box.y; y >= (int)box.y + (int)box.e[3]; y--)
        {
            float3 pixel = { x, y, 0 };
            float3 baryCoords = getBarycentricCoordinates(points, pixel);
            pixel.z = baryCoords.x * points[0].z + baryCoords.y * points[1].z + baryCoords.z * points[2].z;

            if (baryCoords.x >= 0 && baryCoords.y >= 0 && baryCoords.z >= 0 && baryCoords.x <= 1 && baryCoords.y <= 1 && baryCoords.z <= 1 && depthTest(pixel, fb))
            {
                float4 pixelColor = getColor(colors, baryCoords); //{ 1.f, 0.2f, 0.f, 1.f };
                drawPixel(fb.colorBuffer, fb.width, fb.height, x, y, pixelColor);
            }
        }
    }
}

float3 ndcToScreenCoords(float3 ndc, const Viewport& viewport)
{
    return
    {
        viewport.x + (ndc.x +1.f) * (viewport.width) / 2.f,
        viewport.y + (-ndc.y + 1.f) * (viewport.height) / 2.f,
        (ndc.z + 1.f) / 2
    };
}

float3 clipToNdcCoords(float4 clip)
{
    float3 ndc = clip.xyz / clip.w;
    return ndc;
}

bool isOutside(const float4& vertice)
{
    if ((vertice.x < -vertice.w || vertice.x > vertice.w) &&
        (vertice.y < -vertice.w || vertice.y > vertice.w) &&
        (vertice.z < -vertice.w || vertice.z > vertice.w) && vertice.w < 0)
        return true;
    else
        return false;
}

void drawTriangle(rdrImpl* renderer, mat4x4 modelViewProj, rdrVertex* vertices)
{
    // Store triangle vertices positions, 3D locale
    float3 localCoords[3] = {
        { vertices[0].x, vertices[0].y, vertices[0].z },
        { vertices[1].x, vertices[1].y, vertices[1].z },
        { vertices[2].x, vertices[2].y, vertices[2].z },
    };

    // Local space (v3) -> Clip space (v4), 4d (perspective)
    float4 clipCoords[3] = {
        { modelViewProj * float4{ localCoords[0], 1.f } },
        { modelViewProj * float4{ localCoords[1], 1.f } },
        { modelViewProj * float4{ localCoords[2], 1.f } },
    };

    if (isOutside(clipCoords[0]) || isOutside(clipCoords[1]) || isOutside(clipCoords[2]))
        return;

    // Clip space (v4) to NDC (v3) (3d, -1 < n < 1)
    // TODO
    float3 ndcCoords[3] = {
        { clipToNdcCoords(clipCoords[0]) },
        { clipToNdcCoords(clipCoords[1]) },
        { clipToNdcCoords(clipCoords[2]) },
    };

    // NDC (v3) to screen coords (v2) (2d, 0 à size)
    // TODO
    float3 screenCoords[3] = {
        { ndcToScreenCoords(ndcCoords[0], renderer->viewport) },
        { ndcToScreenCoords(ndcCoords[1], renderer->viewport) },
        { ndcToScreenCoords(ndcCoords[2], renderer->viewport) },
    };

    float4 colors[3] = {
        { vertices[0].r, vertices[0].g, vertices[0].b, vertices[0].a },
        { vertices[1].r, vertices[1].g, vertices[1].b, vertices[1].a },
        { vertices[2].r, vertices[2].g, vertices[2].b, vertices[2].a }
    };

    // Rasterize
    /*
    drawLine(renderer->fb, screenCoords[0], screenCoords[1], renderer->lineColor);
    drawLine(renderer->fb, screenCoords[1], screenCoords[2], renderer->lineColor);
    drawLine(renderer->fb, screenCoords[2], screenCoords[0], renderer->lineColor);*/

    fillTriangle(renderer->fb, screenCoords, colors);
}

void rdrDrawTriangles(rdrImpl* renderer, rdrVertex* vertices, int count)
{
    mat4x4 modelViewProj = renderer->projection * renderer->model * renderer->view;//renderer->model;  
    // Transform vertex list to triangles into colorBuffer
    for (int i = 0; i < count; i += 3)
    {
        drawTriangle(renderer, modelViewProj, &vertices[i]);
    }
}

void rdrSetImGuiContext(rdrImpl* renderer, struct ImGuiContext* context)
{
    ImGui::SetCurrentContext(context);
}

void showMatrix(const mat4x4& mat, const char* name)
{
    ImGui::Text(name);
    ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f", mat.e[0], mat.e[1], mat.e[2], mat.e[3]);
    ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f", mat.e[4], mat.e[5], mat.e[6], mat.e[7]);
    ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f", mat.e[8], mat.e[9], mat.e[10], mat.e[11]);
    ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f", mat.e[12], mat.e[13], mat.e[14], mat.e[15]);
}

void rdrShowImGuiControls(rdrImpl* renderer)
{
    ImGui::ColorEdit4("lineColor", renderer->lineColor.e);

    static bool showMVP = false;
    static bool showModel = false;
    ImGui::Checkbox("Show ModelViewProj", &showMVP);
    ImGui::Checkbox("Show Model", &showModel);

    if (showMVP)
    {
        mat4x4 modelViewProj = renderer->projection * renderer->model * renderer->view;
        showMatrix(modelViewProj, "mvp");
    }
    if (showModel)
    {
        showMatrix(renderer->model, "model");
    }

    float FPS = 1 / ImGui::GetIO().DeltaTime;
    ImGui::Text("Frame per second = %5.2f", FPS);
}
