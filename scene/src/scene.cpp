
#include <imgui.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

#include <common/maths.hpp>

#include "scene_impl.hpp"

scnImpl* scnCreate()
{
    return new scnImpl();
}

void scnDestroy(scnImpl* scene)
{
    delete scene;
}

void scnUpdate(scnImpl* scene, float deltaTime, rdrImpl* renderer)
{
    scene->update(deltaTime, renderer);
}

void scnSetImGuiContext(scnImpl* scene, struct ImGuiContext* context)
{
    ImGui::SetCurrentContext(context);
}

void scnShowImGuiControls(scnImpl* scene)
{
    scene->showImGuiControls();
}

void loadObj(char const* inputfile, std::vector<rdrVertex>& vertices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile);

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                rdrVertex vertex = {};
                vertex.x = attrib.vertices[3.f * idx.vertex_index + 0.f];
                vertex.y = attrib.vertices[3.f * idx.vertex_index + 1.f];
                vertex.z = attrib.vertices[3.f * idx.vertex_index + 2.f];
                vertex.r = attrib.normals[3.f * idx.normal_index + 0.f];
                vertex.g = attrib.normals[3.f * idx.normal_index + 1.f];
                vertex.b = attrib.normals[3.f * idx.normal_index + 2.f];
                //vertex.u = attrib.texcoords[2.f * idx.texcoord_index + 0.f];
                //vertex.v = attrib.texcoords[2.f * idx.texcoord_index + 1.f];
                
                //vertex.r = 1.f;//attrib.colors[3 * idx.vertex_index + 0];
                //vertex.g = 1.f;//attrib.colors[3 * idx.vertex_index + 1];
                //vertex.b = 1.f;//attrib.colors[3 * idx.vertex_index + 2];
                vertex.a = 1.f;
                
                vertices.push_back(vertex);
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
}

scnImpl::scnImpl()
{
    //loadObj("assets/boat_large.obj", vertices);
    stbi_ldr_to_hdr_gamma(1.0f);
    texture = stbi_loadf("assets/maxitest.png", &width, &height, nullptr, STBI_rgb_alpha);

    printf("width = %i, height = %i\n", width, height);
    printf("color[0] = {%f, %f, %f, %f}\n", texture[0], texture[1], texture[2], texture[3]);
    
    vertices = {
        //       pos                  normal                    color                   uv
        {-1.0f,-1.0f, 0.0f,      0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f },
        { 1.0f,-1.0f, 0.0f,      0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f },
        { 1.0f, 1.0f, 0.0f,      0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f },
        {-1.0f, 1.0f, 0.0f,      0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f },
        /*
        { 0.5f,-0.5f, 0.0f,      0.0f, 0.0f, 0.0f,      0.0f, 0.0f, 1.0f, 0.5f,     0.0f, 0.0f },
        { 0.0f, 0.5f, 0.0f,      0.0f, 0.0f, 0.0f,      1.0f, 0.0f, 1.0f, 0.5f,     0.0f, 0.0f },
        { 0.0f, 0.5f,-1.5f,      0.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f, 1.0f,     0.0f, 0.0f },
        
        {-0.5f,-0.5f, 0.0f,      0.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f, 1.0f,     0.0f, 0.0f },
        { 0.5f,-0.5f, 0.0f,      0.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f, 1.0f,     0.0f, 0.0f },
        { 0.0f, 0.5f,-1.5f,      0.0f, 0.0f, 0.0f,      1.0f, 0.2f, 0.0f, 1.0f,     0.0f, 0.0f },

        {-0.5f,-0.5f, 0.0f,      0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f },
        { 0.0f, 0.5f, 0.0f,      0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f },
        { 0.0f, 0.5f,-1.5f,      0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f },*/
    };
}

scnImpl::~scnImpl()
{
    // HERE: Unload the scene
}

void scnImpl::update(float deltaTime, rdrImpl* renderer)
{
    // HERE: Update (if needed) and display the scene
    rdrSetTexture(renderer, texture, width, height);

    // Hard coded matrix
    // TODO: Remove this and use proper functions !
    mat4x4 matrix = {
        scale, 0.f, 0.f, 0.f,
        0.f, scale, 0.f, 0.f,
        0.f, 0.f, scale, 0.f,
        0.5f, 0.1f, 0.f, 1.f,
    };

    //matrix = matrix * mat4::rotateY((float)(time * 2.0));

    rdrSetModel(renderer, matrix.e);

    // Draw
    rdrDrawQuads(renderer, vertices.data(), (int)vertices.size());

    time += deltaTime;
}

void scnImpl::showImGuiControls()
{
    ImGui::SliderFloat("scale", &scale, 0.f, 10.f);
    //ImGui::Image(texture, { width, height });
}
