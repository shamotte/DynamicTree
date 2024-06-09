#pragma once

#include "ShaderProgram.h"
#include "Utils.h"

#include <vector>

using namespace glm;

class Camera {
public:
    Camera(vec3 position, vec3 up, float yaw, float pitch);
    mat4 GetViewMatrix(); 
    void ProcessKeyboard(int direction, float deltaTime);

private:
    vec3 Position, Front, Up, Right, WorldUp; // Camera vectors
    float Yaw, Pitch;  // Angles defining the camera's orientation
    float MovementSpeed, Zoom;
    void updateCameraVectors(); // Update camera vectors based on the current yaw and pitch
};