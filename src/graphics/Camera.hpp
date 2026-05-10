#ifndef PAPAYA_GRAPHICS_CAMERA_HPP
#define PAPAYA_GRAPHICS_CAMERA_HPP

#include "core/Types.hpp"

namespace papaya {

/// Camera holding projection and view matrices.
class Camera {
public:
    Camera();

    /// Set perspective projection.
    void set_perspective(f32 fov_y_deg, f32 aspect, f32 near_plane, f32 far_plane);

    /// Set the view matrix directly (e.g. from glm::lookAt).
    void set_view(const Mat4& view) { m_view = view; }

    /// View-projection matrix for use as shader uniform.
    Mat4 view_projection() const { return m_projection * m_view; }

private:
    Mat4 m_projection{1.0f};
    Mat4 m_view{1.0f};
};

} // namespace papaya

#endif // PAPAYA_GRAPHICS_CAMERA_HPP
