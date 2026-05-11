#ifndef PAPAYA_GAMEPLAY_PLAYER_HPP
#define PAPAYA_GAMEPLAY_PLAYER_HPP

#include "core/Types.hpp"

namespace papaya {

struct PlayerMoveInput {
    f32 forward{0.0f};
    f32 right{0.0f};
    f32 up{0.0f};
};

/// Gameplay state for the local player actor.
class Player {
public:
    void update(const PlayerMoveInput& input, f32 camera_yaw, f32 dt);

    const Vec3& position() const { return m_position; }
    Mat4 model_matrix() const;

private:
    Vec3 m_position{0.0f, 1.0f, 0.0f};
    f32  m_speed{5.0f};
};

} // namespace papaya

#endif // PAPAYA_GAMEPLAY_PLAYER_HPP
