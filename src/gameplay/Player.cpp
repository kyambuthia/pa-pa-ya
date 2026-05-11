#include "gameplay/Player.hpp"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace papaya {

void Player::update(const PlayerMoveInput& input, f32 camera_yaw, f32 dt)
{
    if (input.forward != 0.0f || input.right != 0.0f) {
        Vec3 forward_dir{std::sin(camera_yaw), 0.0f, std::cos(camera_yaw)};
        Vec3 right_dir{std::cos(camera_yaw), 0.0f, -std::sin(camera_yaw)};
        m_position += (forward_dir * input.forward + right_dir * input.right) * m_speed * dt;
    }

    m_position.y += input.up * m_speed * dt;
    if (m_position.y < 0.0f) {
        m_position.y = 0.0f;
    }
}

Mat4 Player::model_matrix() const
{
    return glm::translate(Mat4{1.0f}, m_position);
}

} // namespace papaya
