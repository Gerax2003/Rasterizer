
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

void rdrSetViewport(rdrImpl* renderer, int x, int y, int width, int height)
{
    Viewport viewport = { x, y, width, height };
    renderer->viewport = viewport;
}

#pragma region 2D DRAWING FUNCTIONS
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
#pragma endregion

#pragma region LIGHT FACTORS
float getDiffuseFactor(float3 lightDir, float3 normal)
{
    return v3::dotProductVector3(lightDir, normal);
}

float getSpecularFactor(float3 lightDir, float3 normal)
{
    return 0.f;
}
#pragma endregion

#pragma region RASTERIZATION
#pragma region SCREEN BOX
int getLeftPoint(float4* points)
{
    int retNum = INT_MAX;
    for (int i = 0; i < 3; i++)
        if ((int)points[i].x < retNum)
            retNum = (int)points[i].x;

    return retNum;
}

int getRightPoint(float4* points)
{
    int retNum = INT_MIN;
    for (int i = 0; i < 3; i++)
        if ((int)points[i].x > retNum)
            retNum = (int)points[i].x;

    return retNum;
}

int getTopPoint(float4* points)
{
    int retNum = INT_MIN;
    for (int i = 0; i < 3; i++)
        if ((int)points[i].y > retNum)
            retNum = (int)points[i].y;

    return retNum;
}

int getBotPoint(float4* points)
{
    int retNum = INT_MAX;
    for (int i = 0; i < 3; i++)
        if ((int)points[i].y < retNum)
            retNum = (int)points[i].y;

    return retNum;
}

float4 getBoundingBox(float4* points)
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
#pragma endregion

#pragma region TESTS
bool depthTest(float3& point, Framebuffer& buffer)
{
    if (point.x < 0 || point.x >= buffer.width || point.y < 0 || point.y >= buffer.height)
        return false;

    int index = point.x + (point.y * buffer.width);

    if (point.z > buffer.depthBuffer[index])
    {
        buffer.depthBuffer[index] = point.z;
        return true;
    }
    else
        return false;
}

bool checkBaryCoords(float3& baryCoords)
{
    if (baryCoords.x >= 0 && baryCoords.y >= 0 && baryCoords.z >= 0 && baryCoords.x <= 1 && baryCoords.y <= 1 && baryCoords.z <= 1)
        return true;
    else
        return false;
}
#pragma endregion

float3 getBarycentricCoordinates(float4* points, float3 point)
{
    float barycenterDivide = (points[1].y - points[2].y) * (points[0].x - points[2].x) + (points[2].x - points[1].x) * (points[0].y - points[2].y);

    float lambda0 = ((points[1].y - points[2].y) * (point.x - points[2].x) + (points[2].x - points[1].x) * (point.y - points[2].y)) / barycenterDivide;
    float lambda1 = ((points[2].y - points[0].y) * (point.x - points[2].x) + (points[0].x - points[2].x) * (point.y - points[2].y)) / barycenterDivide;
    float lambda2 = 1 - lambda0 - lambda1;

    float perspCorrection = lambda0 * points[0].w + lambda1 * points[1].w + lambda2 * points[2].w;

    lambda0 = (lambda0 * points[0].w) / perspCorrection;
    lambda1 = (lambda1 * points[1].w) / perspCorrection;
    lambda2 = (lambda2 * points[2].w) / perspCorrection;

    return { lambda0, lambda1, lambda2 };
}

float4 getBaryColor(float4* colors, float3 baryCoords)
{
    float r = baryCoords.x * colors[0].r + baryCoords.y * colors[1].r + baryCoords.z * colors[2].r;
    float g = baryCoords.x * colors[0].g + baryCoords.y * colors[1].g + baryCoords.z * colors[2].g;
    float b = baryCoords.x * colors[0].b + baryCoords.y * colors[1].b + baryCoords.z * colors[2].b;
    float a = baryCoords.x * colors[0].a + baryCoords.y * colors[1].a + baryCoords.z * colors[2].a;

    return { r, g, b, a };
}

float4 getWorldPos(Varying* variables, float3 baryCoords)
{
    float x = baryCoords.x * variables[0].worldPos.x + baryCoords.y * variables[1].worldPos.x + baryCoords.z * variables[2].worldPos.x;
    float y = baryCoords.x * variables[0].worldPos.y + baryCoords.y * variables[1].worldPos.y + baryCoords.z * variables[2].worldPos.y;
    float z = baryCoords.x * variables[0].worldPos.z + baryCoords.y * variables[1].worldPos.z + baryCoords.z * variables[2].worldPos.z;
    float w = baryCoords.x * variables[0].worldPos.w + baryCoords.y * variables[1].worldPos.w + baryCoords.z * variables[2].worldPos.w;

    return { x, y, z, w };
}

float3 getNormals(Varying* variables, float3 baryCoords)
{
    float nx = baryCoords.x * variables[0].normals.x + baryCoords.y * variables[1].normals.x + baryCoords.z * variables[2].normals.x;
    float ny = baryCoords.x * variables[0].normals.y + baryCoords.y * variables[1].normals.y + baryCoords.z * variables[2].normals.y;
    float nz = baryCoords.x * variables[0].normals.z + baryCoords.y * variables[1].normals.z + baryCoords.z * variables[2].normals.z;

    return { nx, ny, nz };
}

float3 pixelShader(Uniforms uniforms, Varying var)
{
    float3 kd = { 1.f, 1.f, 1.f };
    float3 ambiantColor = {};
    float3 diffuseColor = {};

    for (int i = 0; i < 8; i++)
        if (uniforms.lights[i].enabled)
        {
            float squaredDistance = v3::squaredLengthV3(uniforms.lights[i].position.xyz - var.worldPos.xyz);
            float attenuation = 1 / (uniforms.lights[i].attenuation.x + uniforms.lights[i].attenuation.y * sqrt(squaredDistance) + uniforms.lights[i].attenuation.z * squaredDistance);

            float3 la = uniforms.lights[i].ambient.xyz *attenuation; //ambient light
            float3 ld = uniforms.lights[i].diffuse.xyz *attenuation; //diffuse light

            float3 l = v3::unitVector3(uniforms.lights[i].position.xyz - var.worldPos.xyz);

            ambiantColor = ambiantColor + la;
            diffuseColor = diffuseColor + kd * getDiffuseFactor(l, var.worldPos.xyz) * ld;
        }

    float3 shadedColor = diffuseColor + ambiantColor;

    for (int i = 0; i < 3; i++)
        if (shadedColor.e[i] > 1.f)
            shadedColor.e[i] = 1.f;

    return shadedColor;
}

Varying getVariables(Varying* variables, float3 baryCoords, Uniforms uniforms)
{
    Varying retVars = {};
    float4 colors[3] = { variables[0].color, variables[1].color, variables[2].color };
    retVars.color = getBaryColor(colors, baryCoords);
    retVars.worldPos = getWorldPos(variables, baryCoords);

    retVars.normals = getNormals(variables, baryCoords);
    
    if (uniforms.pixel)
        retVars.color.xyz = retVars.color.xyz * pixelShader(uniforms, retVars);

    retVars.u = baryCoords.x * variables[0].u + baryCoords.y * variables[1].u + baryCoords.z * variables[2].u;
    retVars.v = baryCoords.x * variables[0].v + baryCoords.y * variables[1].v + baryCoords.z * variables[2].v;

    return retVars;
}

float4 getTexColor(Varying& pixelVariables, rdrImpl* renderer)
{
    if (renderer->uniforms.texture.texWidth <= 0 || renderer->uniforms.texture.texHeight <= 0 || renderer->uniforms.texture.texture == nullptr)
        return pixelVariables.color;

    int texX = (int)(pixelVariables.u * renderer->uniforms.texture.texWidth) % renderer->uniforms.texture.texWidth;
    int texY = renderer->uniforms.texture.texHeight - 1 - ((int)(pixelVariables.v * renderer->uniforms.texture.texHeight) % renderer->uniforms.texture.texHeight);

    int index = (texY * renderer->uniforms.texture.texWidth + texX) * 4;

    float r = pixelVariables.color.r * renderer->uniforms.texture.texture[index + 0];
    float g = pixelVariables.color.g * renderer->uniforms.texture.texture[index + 1];
    float b = pixelVariables.color.b * renderer->uniforms.texture.texture[index + 2];
    float a = pixelVariables.color.a * renderer->uniforms.texture.texture[index + 3];

    return { r, g, b, a };
}

void fillTriangle(rdrImpl* renderer, float4* points, Varying* variables, Uniforms uniforms)
{
    float4 box = getBoundingBox(points);
    
    for (int x = (int)box.x; x <= (int)box.x + (int)box.e[2]; x++)
        for (int y = (int)box.y; y >= (int)box.y + (int)box.e[3]; y--)
        {
            float3 pixel = { x, y, 0 };
            float3 baryCoords = getBarycentricCoordinates(points, pixel);
            pixel.z = baryCoords.x * points[0].z + baryCoords.y * points[1].z + baryCoords.z * points[2].z;

            if (checkBaryCoords(baryCoords) && depthTest(pixel, renderer->fb))
            {
                Varying pixelVariables = getVariables(variables, baryCoords, uniforms);
                float4 color = getTexColor(pixelVariables, renderer);
                drawPixel(renderer->fb.colorBuffer, renderer->fb.width, renderer->fb.height, x, y, color);
            }
        }
}
#pragma endregion

#pragma region TRIANGLE DRAWING
float4 ndcToScreenCoords(float4 ndc, const Viewport& viewport)
{
    return
    {
        viewport.x + (ndc.x +1.f) * (viewport.width) / 2.f,
        viewport.y + (-ndc.y + 1.f) * (viewport.height) / 2.f,
        (-ndc.z + 1.f) / 2, 
        ndc.w
    };
}

float4 clipToNdcCoords(float4 clip)
{
    if (clip.w == 0)
        clip.w = 0.00001f;

    float4 ndc = { clip.xyz / clip.w, 1 / clip.w };
    return ndc;
}

bool clipping(const float4& vertice)
{
    if ((vertice.x < -vertice.w || vertice.x > vertice.w) ||
        (vertice.y < -vertice.w || vertice.y > vertice.w) ||
        (vertice.z < -vertice.w || vertice.z > vertice.w) && vertice.w < 0)
        return true;
    else
        return false;
}

float3 getFaceNormal(rdrVertex* vertex)
{
    float3 vec1 = { vertex[1].x - vertex[0].x, vertex[1].y - vertex[0].y, vertex[1].z - vertex[0].z };
    float3 vec2 = { vertex[2].x - vertex[0].x, vertex[2].y - vertex[0].y, vertex[2].z - vertex[0].z };

    return v3::unitVector3(v3::cross(vec1, vec2));
}

float4 vertexShader(Uniforms uniforms, rdrVertex vertex, Varying& variables)
{
    float3 localNormalPos = { vertex.nx, vertex.ny, vertex.nz };
    float4 worldNormalPos = uniforms.model * float4{ localNormalPos, 0.f };
    float4 worldCoords4 = uniforms.model * float4{ vertex.x, vertex.y, vertex.z, 1.f };

    if (uniforms.gouraud == true)
    {
        float3 kd = { 1.f, 1.f, 1.f };
        float3 ambiantColor = {};
        float3 diffuseColor = {};

        for (int i = 0; i < 8; i++)
            if (uniforms.lights[i].enabled)
            {
                float squaredDistance = v3::squaredLengthV3(uniforms.lights[i].position.xyz - worldCoords4.xyz);
                float attenuation = 1 / (uniforms.lights[i].attenuation.x + uniforms.lights[i].attenuation.y * sqrt(squaredDistance) + uniforms.lights[i].attenuation.z * squaredDistance);
                
                float3 la = uniforms.lights[i].ambient.xyz * attenuation; //ambient light
                float3 ld = uniforms.lights[i].diffuse.xyz * attenuation; //diffuse light
                //float4 cameraPos = uniforms.view * float4(0.f, 0.f, 0.f, 1.f);

                float3 l = v3::unitVector3(uniforms.lights[i].position.xyz - worldCoords4.xyz);

                ambiantColor = ambiantColor + la;
                diffuseColor = diffuseColor + kd * getDiffuseFactor(l, worldNormalPos.xyz) * ld;
            }

        float3 shadedColor = diffuseColor + ambiantColor;

        for (int i = 0; i < 3; i++)
            if (shadedColor.e[i] > 1.f)
                shadedColor.e[i] = 1.f;
    
        variables.color = { vertex.r * shadedColor.x, vertex.g * shadedColor.y, vertex.b * shadedColor.z, vertex.a };
    }
    else
        variables.color = { vertex.r, vertex.g, vertex.b, vertex.a };

    variables.normals = worldNormalPos.xyz;
    variables.u = vertex.u;
    variables.v = vertex.v;
    variables.worldPos = worldCoords4;

    return { uniforms.viewProj * worldCoords4 };
}

void drawTriangle(rdrImpl* renderer, rdrVertex* vertices)
{
    Varying variables[3] = { {}, {}, {} };

    // Local space (v3) -> Clip space (v4), 4d (perspective)
    float4 clipCoords[3] = {
        { vertexShader(renderer->uniforms, vertices[0], variables[0]) },
        { vertexShader(renderer->uniforms, vertices[1], variables[1]) },
        { vertexShader(renderer->uniforms, vertices[2], variables[2]) },
    };

    float3 faceNormal = getFaceNormal(vertices); // TODO: use this for back face culling

    if (clipping(clipCoords[0]) || clipping(clipCoords[1]) || clipping(clipCoords[2]))
        return;

    // Clip space (v4) to NDC (v3) (3d, -1 < n < 1)
    float4 ndcCoords[3] = {
        { clipToNdcCoords(clipCoords[0]) },
        { clipToNdcCoords(clipCoords[1]) },
        { clipToNdcCoords(clipCoords[2]) },
    };

    // NDC (v3) to screen coords (v2) (2d, 0 à size)
    float4 screenCoords[3] = {
        { ndcToScreenCoords(ndcCoords[0], renderer->viewport) },
        { ndcToScreenCoords(ndcCoords[1], renderer->viewport) },
        { ndcToScreenCoords(ndcCoords[2], renderer->viewport) },
    };

    if (renderer->uniforms.wireframe)
    {
        drawLine(renderer->fb, screenCoords[0].xyz, screenCoords[1].xyz, renderer->lineColor);
        drawLine(renderer->fb, screenCoords[1].xyz, screenCoords[2].xyz, renderer->lineColor);
        drawLine(renderer->fb, screenCoords[2].xyz, screenCoords[0].xyz, renderer->lineColor);
    }
    else
        fillTriangle(renderer, screenCoords, variables, renderer->uniforms);
}
#pragma endregion

#pragma region 3D DRAWING FUNCTIONS
void rdrDrawTriangles(rdrImpl* renderer, rdrVertex* vertices, int count)
{
    renderer->uniforms.modelViewProj = renderer->uniforms.proj * renderer->uniforms.model * renderer->uniforms.view;
    renderer->uniforms.viewProj =  renderer->uniforms.proj * renderer->uniforms.view;
    // Transform vertex list to triangles into colorBuffer
    for (int i = 0; i < count; i += 3)
    {
        drawTriangle(renderer, &vertices[i]);
    }
}

void rdrDrawQuads(rdrImpl* renderer, rdrVertex* vertices, int vertexCount)
{
    renderer->uniforms.modelViewProj = renderer->uniforms.proj * renderer->uniforms.model * renderer->uniforms.view;
    renderer->uniforms.viewProj = renderer->uniforms.view * renderer->uniforms.proj;
    // Transform vertex list to triangles into colorBuffer
    for (int i = 0; i < vertexCount; i += 4)
    {
        drawTriangle(renderer, &vertices[i]);
        rdrVertex tempVertices[3] = { vertices[i + 2], vertices[i + 3], vertices[i] };
        drawTriangle(renderer, tempVertices);
    }
}
#pragma endregion

#pragma region UNIFORMS
void rdrSetUniformFloatV(rdrImpl* renderer, rdrUniformType type, float* value)
{
    switch (type)
    {
    case UT_TIME:      renderer->uniforms.time = value[0]; break;
    case UT_DELTATIME: renderer->uniforms.deltaTime = value[0]; break;
    default:;
    }
}

void rdrSetUniformBoolV(rdrImpl* renderer, rdrUniformType type, bool value)
{
    switch (type)
    {
    case UT_GOURAUD: renderer->uniforms.gouraud = value; break;
    case UT_PIXEL: renderer->uniforms.pixel = value; break;
    case UT_WIREFRAME: renderer->uniforms.wireframe = value; break;
    default:;
    }
}

void rdrSetUniformLight(rdrImpl* renderer, int index, rdrLight* light)
{
    if (index < 0 || index >= IM_ARRAYSIZE(renderer->uniforms.lights))
        return;

    memcpy(&renderer->uniforms.lights[index], light, sizeof(rdrLight));
}

void rdrSetTexture(rdrImpl* renderer, float* colors32Bits, int width, int height)
{
    renderer->uniforms.texture = { colors32Bits, width, height };
}

void rdrSetUniformMaterial(rdrImpl* renderer, rdrMaterial* material)
{
    if (material == nullptr)
        return;

    memcpy(&renderer->uniforms.material, material, sizeof(rdrMaterial));
}

void rdrSetProjection(rdrImpl* renderer, float* projectionMatrix)
{
    memcpy(renderer->uniforms.proj.e, projectionMatrix, 16 * sizeof(float));
}

void rdrSetView(rdrImpl* renderer, float* viewMatrix)
{
    memcpy(renderer->uniforms.view.e, viewMatrix, 16 * sizeof(float));
}

void rdrSetModel(rdrImpl* renderer, float* modelMatrix)
{
    memcpy(renderer->uniforms.model.e, modelMatrix, 16 * sizeof(float));
}
#pragma endregion

#pragma region IMGUI
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

    ImGui::Checkbox("Gouraud shading", &renderer->uniforms.gouraud);
    ImGui::Checkbox("Pixel shading", &renderer->uniforms.pixel);
    ImGui::Checkbox("Wireframe shading", &renderer->uniforms.wireframe);

    ImGui::Checkbox("Show ModelViewProj", &showMVP);
    ImGui::Checkbox("Show Model", &showModel);

    if (showMVP)
        showMatrix(renderer->uniforms.modelViewProj, "mvp");
    if (showModel)
        showMatrix(renderer->uniforms.model, "model");
}
#pragma endregion
