#ifndef PAPAYA_GAMEPLAY_PLAYER_HPP
#define PAPAYA_GAMEPLAY_PLAYER_HPP

#include "core/Types.hpp"

namespace papaya {

struct PlayerMoveInput {
    f32 forward{0.0f};
    f32 right{0.0f};
    f32 up{0.0f};
};

enum class PlayerMode {
    Walker,
    Vehicle,
};

enum class PlayerTransformState {
    Walker,
    TransformingToVehicle,
    Vehicle,
    TransformingToWalker,
};

/// Gameplay state for the local player actor.
class Player {
public:
    void update(const PlayerMoveInput& input, f32 camera_yaw, f32 dt);
    void toggle_mode();

    const Vec3& position() const { return m_position; }
    Vec3 focus_position() const;
    Mat4 render_matrix(PlayerMode mode) const;
    PlayerMode mode() const;
    PlayerTransformState transform_state() const { return m_state; }
    f32 vehicle_blend() const;

private:
    void update_transform(f32 dt);

    Vec3 m_position{0.0f, 0.0f, 0.0f};
    PlayerTransformState m_state{PlayerTransformState::Walker};
    f32 m_transform_elapsed{0.0f};
    f32 m_transform_duration{0.35f};
    f32 m_walker_speed{5.0f};
    f32 m_vehicle_speed{10.0f};
};

} // namespace papaya

#endif // PAPAYA_GAMEPLAY_PLAYER_HPP
