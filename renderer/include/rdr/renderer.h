#pragma once

#ifdef RDR_EXPORTS
#define RDR_API __declspec(dllexport)
#else
#define RDR_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// Opaque struct to store our data privately
typedef struct rdrImpl rdrImpl;

// Vertex format (only one supported for now)
typedef struct rdrVertex
{
    float x, y, z;    // Pos
    float nx, ny, nz; // Normal
    float r, g, b, a; // Color
    float u, v;       // Texture coordinates
} rdrVertex;

typedef struct rdrLight
{
    bool enabled;
    float position[4]; // world pos
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float attenuation[3];
} rdrLight;

typedef struct rdrMaterial
{
    float ambientColor[4];
    float diffuseColor[4];
    float specularColor[4];
    float emissionColor[4];
    float shininess;
} rdrMaterial;

enum rdrUniformType
{
    UT_TIME,      
    UT_DELTATIME, 
    UT_CAMERA_POS,

    UT_USER = 100,
    UT_GOURAUD,
    UT_PIXEL,
    UT_WIREFRAME,
    UT_TEXTURE,
    UT_DEPTH_BUFFER,
};


// Init/Shutdown function
// Color and depth buffer have to be valid until the shutdown of the renderer
// Color buffer is RGBA, each component is a 32 bits float
// Depth buffer is a buffer of 32bits floats
RDR_API rdrImpl* rdrInit(float* colorBuffer32Bits, float* depthBuffer, int width, int height);
RDR_API void rdrShutdown(rdrImpl* renderer);
RDR_API void rdrFinish(rdrImpl* renderer);

// Matrix setup
RDR_API void rdrSetProjection(rdrImpl* renderer, float* projectionMatrix);
RDR_API void rdrSetView(rdrImpl* renderer, float* viewMatrix);
RDR_API void rdrSetModel(rdrImpl* renderer, float* modelMatrix);
RDR_API void rdrSetViewport(rdrImpl* renderer, int x, int y, int width, int height);

// Texture setup
RDR_API void rdrSetTexture(rdrImpl* renderer, float* colors32Bits, int width, int height);

// Draw a list of triangles
RDR_API void rdrDrawTriangles(rdrImpl* renderer, rdrVertex* vertices, int vertexCount);
RDR_API void rdrDrawQuads(rdrImpl* renderer, rdrVertex* vertices, int vertexCount);

// Modify uniforms
RDR_API void rdrSetUniformFloatV(rdrImpl* renderer, rdrUniformType type, float* value);
RDR_API void rdrSetUniformBoolV(rdrImpl* renderer, rdrUniformType type, bool value);
RDR_API void rdrSetUniformLight(rdrImpl* renderer, int index, rdrLight* light);
RDR_API void rdrSetUniformMaterial(rdrImpl* renderer, rdrMaterial* material);

struct ImGuiContext;
RDR_API void rdrSetImGuiContext(rdrImpl* renderer, struct ImGuiContext* context);
RDR_API void rdrShowImGuiControls(rdrImpl* renderer);

#ifdef __cplusplus
}
#endif
