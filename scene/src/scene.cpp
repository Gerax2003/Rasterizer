
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
                vertex.nx = attrib.normals[3.f * idx.normal_index + 0.f];
                vertex.ny = attrib.normals[3.f * idx.normal_index + 1.f];
                vertex.nz = attrib.normals[3.f * idx.normal_index + 2.f];
                vertex.u = attrib.texcoords[2.f * idx.texcoord_index + 0.f];
                vertex.v = attrib.texcoords[2.f * idx.texcoord_index + 1.f];
                
                vertex.r = 1.f;//attrib.colors[3 * idx.vertex_index + 0];
                vertex.g = 1.f;//attrib.colors[3 * idx.vertex_index + 1];
                vertex.b = 1.f;//attrib.colors[3 * idx.vertex_index + 2];
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
    loadObj("assets/Tests/boat_large.obj", vertices);
    stbi_ldr_to_hdr_gamma(1.0f);
    texture = stbi_loadf("assets/Tests/minitest.png", &width, &height, nullptr, STBI_rgb_alpha);

    printf("width = %i, height = %i\n", width, height);
    /*
    vertices = {
        //       pos                  normal                    color                   uv
        {-1.0f,-1.0f, 0.0f,      0.0f, 0.0f, 1.0f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f },
        { 1.0f,-1.0f, 0.0f,      0.0f, 0.0f, 1.0f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f },
        { 1.0f, 1.0f, 0.0f,      0.0f, 0.0f, 1.0f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f },
        {-1.0f, 1.0f, 0.0f,      0.0f, 0.0f, 1.0f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f },

        {-1.0f,-1.0f,-2.0f,      0.0f,-1.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f },
        { 1.0f,-1.0f,-2.0f,      0.0f,-1.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f },
        { 1.0f,-1.0f, 0.0f,      0.0f,-1.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f },
        {-1.0f,-1.0f, 0.0f,      0.0f,-1.0f, 0.0f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f },
    };*/
}

scnImpl::~scnImpl()
{
    // HERE: Unload the scene
}

void scnImpl::update(float deltaTime, rdrImpl* renderer)
{
    // HERE: Update (if needed) and display the scene
    //rdrSetTexture(renderer, texture, width, height);

    mat4x4 matrix = mat4::scale(scale);
    matrix = matrix * mat4::rotateX(rotateX) * mat4::rotateY(rotateY) * mat4::rotateZ(rotateZ);

    rdrSetModel(renderer, matrix.e);

    // Draw
    rdrDrawTriangles(renderer, vertices.data(), (int)vertices.size());

    time += deltaTime;
}

void scnImpl::showImGuiControls()
{
    ImGui::SliderFloat("scale", &scale, 0.f, 10.f);
    ImGui::SliderFloat("Rotation x", &rotateX, 0.f, 3.1415f);
    ImGui::SliderFloat("Rotation y", &rotateY, 0.f, 3.1415f);
    ImGui::SliderFloat("Rotation z", &rotateZ, 0.f, 3.1415f);
}
