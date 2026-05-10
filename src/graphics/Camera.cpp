#include "graphics/Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace papaya {

Camera::Camera()
{
    set_perspective(60.0f, 16.0f / 9.0f, 0.1f, 100.0f);
}

void Camera::set_perspective(f32 fov_y_deg, f32 aspect, f32 near_plane, f32 far_plane)
{
    m_projection = glm::perspective(glm::radians(fov_y_deg), aspect, near_plane, far_plane);
}

} // namespace papaya
