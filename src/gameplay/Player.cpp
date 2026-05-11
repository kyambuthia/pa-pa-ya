#include "gameplay/Player.hpp"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace papaya {

void Player::toggle_mode()
{
    switch (m_state) {
        case PlayerTransformState::Walker:
            m_state = PlayerTransformState::TransformingToVehicle;
            m_transform_elapsed = 0.0f;
            break;

        case PlayerTransformState::Vehicle:
            m_state = PlayerTransformState::TransformingToWalker;
            m_transform_elapsed = 0.0f;
            break;

        case PlayerTransformState::TransformingToVehicle:
            m_state = PlayerTransformState::TransformingToWalker;
            m_transform_elapsed = m_transform_duration - m_transform_elapsed;
            break;

        case PlayerTransformState::TransformingToWalker:
            m_state = PlayerTransformState::TransformingToVehicle;
            m_transform_elapsed = m_transform_duration - m_transform_elapsed;
            break;
    }
}

void Player::update_transform(f32 dt)
{
    switch (m_state) {
        case PlayerTransformState::TransformingToVehicle:
        case PlayerTransformState::TransformingToWalker:
            m_transform_elapsed += dt;
            if (m_transform_elapsed >= m_transform_duration) {
                m_transform_elapsed = m_transform_duration;
                m_state = (m_state == PlayerTransformState::TransformingToVehicle)
                    ? PlayerTransformState::Vehicle
                    : PlayerTransformState::Walker;
            }
            break;

        case PlayerTransformState::Walker:
        case PlayerTransformState::Vehicle:
            break;
    }
}

f32 Player::vehicle_blend() const
{
    if (m_transform_duration <= 0.0f) {
        return (m_state == PlayerTransformState::Vehicle) ? 1.0f : 0.0f;
    }

    const f32 t = glm::clamp(m_transform_elapsed / m_transform_duration, 0.0f, 1.0f);
    switch (m_state) {
        case PlayerTransformState::Walker:
            return 0.0f;

        case PlayerTransformState::TransformingToVehicle:
            return t;

        case PlayerTransformState::Vehicle:
            return 1.0f;

        case PlayerTransformState::TransformingToWalker:
            return 1.0f - t;
    }

    return 0.0f;
}

PlayerMode Player::mode() const
{
    switch (m_state) {
        case PlayerTransformState::Vehicle:
        case PlayerTransformState::TransformingToVehicle:
            return PlayerMode::Vehicle;

        case PlayerTransformState::Walker:
        case PlayerTransformState::TransformingToWalker:
            return PlayerMode::Walker;
    }

    return PlayerMode::Walker;
}

void Player::update(const PlayerMoveInput& input, f32 camera_yaw, f32 dt)
{
    update_transform(dt);

    const f32 speed = glm::mix(m_walker_speed, m_vehicle_speed, vehicle_blend());

    if (input.forward != 0.0f || input.right != 0.0f) {
        Vec3 forward_dir{std::sin(camera_yaw), 0.0f, std::cos(camera_yaw)};
        Vec3 right_dir{std::cos(camera_yaw), 0.0f, -std::sin(camera_yaw)};
        m_position += (forward_dir * input.forward + right_dir * input.right) * speed * dt;
    }

    m_position.y += input.up * speed * dt;
    if (m_position.y < 0.0f) {
        m_position.y = 0.0f;
    }
}

Vec3 Player::focus_position() const
{
    const f32 focus_height = glm::mix(1.0f, 0.55f, vehicle_blend());
    return m_position + Vec3{0.0f, focus_height, 0.0f};
}

Mat4 Player::render_matrix(PlayerMode mode) const
{
    const f32 center_height = (mode == PlayerMode::Walker) ? 1.0f : 0.4f;
    return glm::translate(Mat4{1.0f}, m_position + Vec3{0.0f, center_height, 0.0f});
}

} // namespace papaya
