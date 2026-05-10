#ifndef PAPAYA_GRAPHICS_CAMERA_HPP
#define PAPAYA_GRAPHICS_CAMERA_HPP

#include "core/Types.hpp"

namespace papaya {

/// FPS-style camera with yaw/pitch controls.
class Camera {
public:
    Camera();

    /// Set perspective projection.
    void set_perspective(f32 fov_y_deg, f32 aspect, f32 near_plane, f32 far_plane);

    /// Move relative to current orientation (forward/right/up).
    void move(f32 forward, f32 right, f32 up, f32 dt);

    /// Rotate by mouse deltas (pitch, yaw in radians or pixels).
    void look(f32 pitch_delta, f32 yaw_delta);

    /// View-projection matrix for use as shader uniform.
    Mat4 view_projection() const;

    /// Current world-space position.
    const Vec3& position() const { return m_position; }

private:
    Vec3 m_position{0.0f, 2.0f, 5.0f};
    f32  m_yaw{0.0f};
    f32  m_pitch{-0.2618f};  // -15° in radians, looking slightly down

    f32 m_move_speed{6.0f};
    f32 m_look_sensitivity{0.002f};

    Mat4 m_projection{1.0f};
};

} // namespace papaya

#endif // PAPAYA_GRAPHICS_CAMERA_HPP
