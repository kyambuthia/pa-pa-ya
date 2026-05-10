#include "graphics/Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace papaya {

Camera::Camera()
{
    set_perspective(60.0f, 16.0f / 9.0f, 0.1f, 100.0f);
}

void Camera::set_perspective(f32 fov_y_deg, f32 aspect, f32 near_plane, f32 far_plane)
{
    m_projection = glm::perspective(glm::radians(fov_y_deg), aspect, near_plane, far_plane);
}

void Camera::move(f32 forward, f32 right, f32 up, f32 dt)
{
    // Forward direction from yaw (ignore pitch for movement)
    Vec3 f_dir{std::sin(m_yaw), 0.0f, std::cos(m_yaw)};
    Vec3 r_dir{std::cos(m_yaw), 0.0f, -std::sin(m_yaw)};
    Vec3 u_dir{0.0f, 1.0f, 0.0f};

    m_position += (f_dir * forward + r_dir * right + u_dir * up) * m_move_speed * dt;
}

void Camera::look(f32 pitch_delta, f32 yaw_delta)
{
    m_yaw   += yaw_delta   * m_look_sensitivity;
    m_pitch += pitch_delta * m_look_sensitivity;

    // Clamp pitch to avoid gimbal lock
    constexpr f32 PITCH_LIMIT = glm::radians(89.0f);
    m_pitch = glm::clamp(m_pitch, -PITCH_LIMIT, PITCH_LIMIT);
}

Mat4 Camera::view_projection() const
{
    Vec3 f_dir{
        std::sin(m_yaw) * std::cos(m_pitch),
        std::sin(m_pitch),
        std::cos(m_yaw) * std::cos(m_pitch)
    };
    f_dir = glm::normalize(f_dir);

    return m_projection * glm::lookAt(m_position, m_position + f_dir, Vec3{0.0f, 1.0f, 0.0f});
}

} // namespace papaya
