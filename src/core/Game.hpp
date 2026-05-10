#ifndef PAPAYA_CORE_GAME_HPP
#define PAPAYA_CORE_GAME_HPP

#include "Types.hpp"
#include "Log.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"

#include <sokol_gfx.h>
#include <sokol_app.h>
#include <sokol_glue.h>
#include <sokol_log.h>
#include <sokol_time.h>

#include <array>

namespace papaya {

/// Top-level game application class.
/// sokol drives the main loop; this class holds all game state
/// and is called from the static sapp callbacks.
class Game {
public:
    Game();
    ~Game();

    /// Initialise subsystems, shaders, meshes.
    bool init();

    /// Called every frame by sapp (frame_cb).
    void update_frame();

    /// Shut down all subsystems (cleanup_cb).
    void shutdown();

    /// Handle input events from sapp (event_cb).
    void handle_event(const sapp_event* e);

    /// Singleton access for sapp callbacks.
    static Game* instance() { return s_instance; }

    // Not copyable / movable
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

private:
    bool create_shader();
    bool create_pipelines();
    bool create_scene_meshes();

    // ── Pass action ───────────────────────────────────────────
    sg_pass_action m_pass_action{};

    // ── Shader & pipelines ────────────────────────────────────
    sg_shader   m_shader{};
    sg_pipeline m_pip_triangles{};
    sg_pipeline m_pip_lines{};

    // ── Scene objects ─────────────────────────────────────────
    Camera m_camera;
    Mesh   m_capsule;
    Mesh   m_grid;

    // ── Player state ──────────────────────────────────────────
    Vec3 m_player_pos{0.0f, 1.0f, 0.0f};  // centre of capsule
    f32  m_player_speed{5.0f};

    // ── Camera orbit ──────────────────────────────────────────
    f32 m_cam_yaw{0.0f};
    f32 m_cam_pitch{glm::radians(-15.0f)};
    f32 m_cam_distance{6.0f};
    f32 m_cam_look_sensitivity{0.003f};

    // ── Input state ───────────────────────────────────────────
    std::array<bool, SAPP_KEYCODE_MENU + 1> m_keys_held{};
    bool m_mouse_locked{true};

    // ── Timing ────────────────────────────────────────────────
    uint64_t m_last_tick{0};

    static Game* s_instance;
};

} // namespace papaya

#endif // PAPAYA_CORE_GAME_HPP
