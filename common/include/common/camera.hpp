#pragma once

#include <common/types.hpp>

struct CameraInputs
{
    float deltaX;
    float deltaY;
    bool moveForward;
    bool moveBackward;
    // ... and more if needed
};

struct Camera
{
    Camera(int width, int height);

    void update(float deltaTime, const CameraInputs& inputs);
    mat4x4 getViewMatrix();
    mat4x4 getProjection();

    void showImGuiControls();

    float yaw = 0;
    float pitch = 0;
    float3 position = { 0, 0, 5.0f };

    float aspect;
    float fovY = 80;
    float near = 0.001f;
    float far = 100.f;
};