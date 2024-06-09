#include "Camera.h"



Camera::Camera(vec3 position, vec3 up, float yaw, float pitch) 
    : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch), MovementSpeed(2.5f), Zoom(45.0f) {
    updateCameraVectors();
}

mat4 Camera::GetViewMatrix() {
    return lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == GLFW_KEY_W)
        Position += Front * velocity;  // Move forward
    if (direction == GLFW_KEY_S)
        Position -= Front * velocity; // Move backward
    if (direction == GLFW_KEY_A)
        Position -= Right * velocity; // Move left
    if (direction == GLFW_KEY_D)
        Position += Right * velocity; // Move right
    if (direction == GLFW_KEY_UP)
        Position += Up * velocity; // Move up
    if (direction == GLFW_KEY_DOWN)
        Position -= Up * velocity; // Move down
    if (direction == GLFW_KEY_LEFT)
        Yaw -= velocity * 25.0f; // Rotate left (imitate head turn) 
    if (direction == GLFW_KEY_RIGHT)
        Yaw += velocity * 25.0f; // Rotate right (imitate head turn)
    updateCameraVectors();
}

// Update camera vectors based on the current yaw and pitch
void Camera::updateCameraVectors() {
    vec3 front;
    front.x = cos(radians(Yaw)) * cos(radians(Pitch)); //Yaw - y-axis rotation
    front.y = sin(radians(Pitch));                     //Pitch - x-axis rotation
    front.z = sin(radians(Yaw)) * cos(radians(Pitch)); 
    Front = normalize(front);
    Right = normalize(cross(Front, WorldUp));
    Up = normalize(cross(Right, Front));
}