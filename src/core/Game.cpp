#include "core/Game.hpp"

#include "core/Log.hpp"

#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_log.h>
#include <sokol_time.h>

namespace papaya {

// ── Statics ─────────────────────────────────────────────────────

Game* Game::s_instance = nullptr;

// ── Constructor / Destructor ────────────────────────────────────

Game::Game()
{
    PAPAYA_ASSERT(s_instance == nullptr, "Only one Game instance allowed");
    s_instance = this;
}

Game::~Game()
{
    s_instance = nullptr;
}

// ── Initialisation ──────────────────────────────────────────────

bool Game::init()
{
    PAPAYA_TRACE("Game::init()");

    // ── timing ───────────────────────────────────────────────
    stm_setup();
    m_last_tick = stm_now();

    // ── sokol gfx setup ─────────────────────────────────────
    sg_desc desc{};
    desc.environment = sglue_environment();
    desc.logger.func = slog_func;
    sg_setup(&desc);

    if (!sg_isvalid()) {
        PAPAYA_ERROR("sg_setup failed");
        return false;
    }

    if (!m_world.init()) return false;
    if (!m_renderer.init()) return false;

    // ── camera ───────────────────────────────────────────────
    int w = sapp_width();
    int h = sapp_height();
    m_camera.set_perspective(60.0f, static_cast<f32>(w) / static_cast<f32>(h), 0.1f, 100.0f);

    // ── mouse capture ────────────────────────────────────────
    sapp_lock_mouse(true);
    m_mouse_locked = true;

    PAPAYA_INFO("Game initialised successfully");
    return true;
}

// ── Frame update ────────────────────────────────────────────────

void Game::update_frame()
{
    // ── delta time ───────────────────────────────────────────
    u64 now = stm_now();
    f32 dt = static_cast<f32>(stm_sec(now - m_last_tick));
    m_last_tick = now;

    PlayerMoveInput move_input;
    if (m_keys_held[SAPP_KEYCODE_W]) move_input.forward += 1.0f;
    if (m_keys_held[SAPP_KEYCODE_S]) move_input.forward -= 1.0f;
    if (m_keys_held[SAPP_KEYCODE_A]) move_input.right -= 1.0f;
    if (m_keys_held[SAPP_KEYCODE_D]) move_input.right += 1.0f;
    if (m_keys_held[SAPP_KEYCODE_SPACE])      move_input.up += 1.0f;
    if (m_keys_held[SAPP_KEYCODE_LEFT_SHIFT]) move_input.up -= 1.0f;

    m_player.update(move_input, m_cam_yaw, dt);

    // ── camera orbit ────────────────────────────────────────
    Vec3 offset;
    offset.x = m_cam_distance * std::sin(m_cam_yaw) * std::cos(m_cam_pitch);
    offset.y = m_cam_distance * std::sin(m_cam_pitch);
    offset.z = m_cam_distance * std::cos(m_cam_yaw) * std::cos(m_cam_pitch);

    Vec3 cam_pos = m_player.position() + offset;
    m_camera.set_view(glm::lookAt(cam_pos, m_player.position(), Vec3{0.0f, 1.0f, 0.0f}));

    m_renderer.draw(m_camera, m_world, m_player);
}

// ── Shutdown ────────────────────────────────────────────────────

void Game::shutdown()
{
    PAPAYA_TRACE("Game::shutdown()");

    m_renderer.shutdown();
    m_world.shutdown();

    sg_shutdown();
}

// ── Input ───────────────────────────────────────────────────────

void Game::handle_event(const sapp_event* e)
{
    switch (e->type) {

        case SAPP_EVENTTYPE_KEY_DOWN:
            if (e->key_code < m_keys_held.size()) {
                m_keys_held[e->key_code] = true;
            }
            if (e->key_code == SAPP_KEYCODE_ESCAPE) {
                sapp_request_quit();
            }
            if (e->key_code == SAPP_KEYCODE_TAB) {
                m_mouse_locked = !m_mouse_locked;
                sapp_lock_mouse(m_mouse_locked);
            }
            break;

        case SAPP_EVENTTYPE_KEY_UP:
            if (e->key_code < m_keys_held.size()) {
                m_keys_held[e->key_code] = false;
            }
            break;

        case SAPP_EVENTTYPE_MOUSE_MOVE:
            if (m_mouse_locked) {
                m_cam_yaw   += static_cast<f32>(e->mouse_dx) * m_cam_look_sensitivity;
                m_cam_pitch += static_cast<f32>(e->mouse_dy) * m_cam_look_sensitivity;

                constexpr f32 PITCH_LIMIT = glm::radians(85.0f);
                m_cam_pitch = glm::clamp(m_cam_pitch, -PITCH_LIMIT, PITCH_LIMIT);
            }
            break;

        case SAPP_EVENTTYPE_RESIZED: {
            int w = e->window_width;
            int h = e->window_height;
            if (w > 0 && h > 0) {
                m_camera.set_perspective(60.0f, static_cast<f32>(w) / static_cast<f32>(h), 0.1f, 100.0f);
            }
            break;
        }

        default:
            break;
    }
}

} // namespace papaya
