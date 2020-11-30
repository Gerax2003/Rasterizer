#pragma once

#include <common/types.hpp>

struct CameraInputs
{
    float deltaX;
    float deltaY;
    bool moveForward;
    bool moveBackward;
    bool moveLeft;
    bool moveRight;
    bool moveUpwards;
    bool moveDownwards;
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
    float3 position = { 0, 0, 10.0f };

    float aspect;
    float fovY = 80;
    float near = 0.15f;
    float far = 75.f;
};