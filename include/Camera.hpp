#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// The camera class
class Camera {
public:
    Camera(const glm::vec3& position = { 0.f, 0.f, 3.f }, float pitch = 0.0f, float yaw = -90.0f, float fov = 90.0f)
        : m_Position(position), m_Pitch(pitch), m_Yaw(yaw), m_FOV(fov) {
        ClampPitch();
        Update();
    }
    // Getting the camera's view matrix
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(m_Position, m_Position + m_Front, WORLD_UP);
    }
    // Getting the camera's position
    const glm::vec3& GetPosition() const {
        return m_Position;
    }
    // Setting the camera's position
    void SetPosition(const glm::vec3& position) {
        m_Position = position;
    }
    // Getting the camera's front direction
    const glm::vec3& GetFront() const {
        return m_Front;
    }
    // Getting the camera;s right direction
    const glm::vec3& GetRight() const {
        return m_Right;
    }
    // Getting the camera's up direction
    const glm::vec3& GetUp() const {
        return m_Up;
    }
    const float& GetFOV() const {
        return m_FOV;
    }
    void SetFOV(float fov) {
        m_FOV = fov;
    }
    // Getting the pitch(x rotation)
    const float& GetPitch() const {
        return m_Pitch;
    }
    // Setting the pitch(x rotation)
    void SetPitch(float pitch) {
        m_Pitch = pitch;
        ClampPitch();
        Update();
    }
    // Getting the yaw(y rotation)
    const float& GetYaw() const {
        return m_Yaw;
    }
    // Setting the yaw(y rotation)
    void SetYaw(float yaw) {
        m_Yaw = yaw;
#ifdef CLAMP_YAW_TO_360_DEG
        ClampYaw();
#endif
        Update();
    }
    // The worlds up direction
    static constexpr glm::vec3 WORLD_UP{ 0.f, 1.f, 0.f };
private:
    void Update() {
        m_Front = glm::normalize(
            glm::vec3(
                glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch)),
                glm::sin(glm::radians(m_Pitch)),
                glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch))
            )
        );
        m_Right = glm::normalize(glm::cross(m_Front, WORLD_UP));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }
    void ClampPitch() {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.f)
            m_Pitch = -89.f;
    }
    void ClampYaw() {
        float remainder = (int)glm::abs(m_Yaw) % 360;
        remainder = glm::abs(m_Yaw) - remainder;
        if (m_Yaw > 360.f) {
            m_Yaw = remainder;
        }
        if (m_Yaw < 0.f) {
            m_Yaw = 360.f - remainder;
        }
    }
private:
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Right;
    glm::vec3 m_Up;
    float m_Pitch;
    float m_Yaw;
    float m_Roll;
    float m_FOV;
};
