#ifndef PAPAYA_CORE_GAME_HPP
#define PAPAYA_CORE_GAME_HPP

#include "Types.hpp"
#include "gameplay/Player.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Renderer.hpp"
#include "world/World.hpp"

#include <sokol_app.h>

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
    // ── Gameplay / world / rendering ──────────────────────────
    World    m_world;
    Player   m_player;
    Camera   m_camera;
    Renderer m_renderer;

    // ── Camera orbit ──────────────────────────────────────────
    f32 m_cam_yaw{0.0f};
    f32 m_cam_pitch{glm::radians(-15.0f)};
    f32 m_cam_distance{6.0f};
    f32 m_cam_look_sensitivity{0.003f};

    // ── Input state ───────────────────────────────────────────
    std::array<bool, SAPP_KEYCODE_MENU + 1> m_keys_held{};
    bool m_mouse_locked{true};

    // ── Timing ────────────────────────────────────────────────
    u64 m_last_tick{0};

    static Game* s_instance;
};

} // namespace papaya

#endif // PAPAYA_CORE_GAME_HPP
