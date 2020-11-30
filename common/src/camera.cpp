
#include <imgui.h>

#include <common/maths.hpp>

#include <common/camera.hpp>

Camera::Camera(int width, int height)
{
    aspect = (float)width / (float)height;
}

void Camera::update(float deltaTime, const CameraInputs& inputs)
{
    const float MOUSE_SENSITIVITY = 0.002f;
    const float SPEED = 5.f;

    yaw += inputs.deltaX * MOUSE_SENSITIVITY;
    pitch += inputs.deltaY * MOUSE_SENSITIVITY;

    float forward_movement = 0.f;
    if (inputs.moveForward)
        forward_movement -= SPEED * deltaTime;
    if (inputs.moveBackward)
        forward_movement += SPEED * deltaTime;

    position.x += maths::sin(yaw) * forward_movement;
    position.z += maths::cos(yaw) * forward_movement;
}

mat4x4 Camera::getViewMatrix()
{
    return mat4::rotateX(pitch) * mat4::rotateY(-yaw) * mat4::translate(-position);
    //return mat4::identity();
}

mat4x4 Camera::getProjection()
{
    return mat4::perspective(fovY, aspect, near, far);
    //return mat4::identity();
}

void showMatrix(const mat4x4& mat, const char* name)
{
    ImGui::Text(name);
    ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f", mat.e[0], mat.e[1], mat.e[2], mat.e[3]);
    ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f", mat.e[4], mat.e[5], mat.e[6], mat.e[7]);
    ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f", mat.e[8], mat.e[9], mat.e[10], mat.e[11]);
    ImGui::Text("%5.2f, %5.2f, %5.2f, %5.2f", mat.e[12], mat.e[13], mat.e[14], mat.e[15]);
}

void Camera::showImGuiControls()
{
    ImGui::SliderFloat3("pos", position.e, -5.f, 5.f);
    ImGui::SliderFloat("yaw", &yaw, -3.14159f, 3.14159f);
    ImGui::SliderFloat("pitch", &pitch, -3.14159f, 3.14159f);
    if (ImGui::Button("Reset pos"))
        position = { 0.f, 0.f, 1.f };
    
    if (ImGui::Button("Reset rotation"))
    {
        yaw = 0.f;
        pitch = 0.f;
    }

    showMatrix(getProjection(), "proj");
    showMatrix(getViewMatrix(), "view");
}