#ifndef PAPAYA_CORE_GAME_HPP
#define PAPAYA_CORE_GAME_HPP

#include "Types.hpp"
#include "Log.hpp"

#include <sokol_gfx.h>
#include <sokol_app.h>
#include <sokol_glue.h>
#include <sokol_log.h>
#include <sokol_time.h>

namespace papaya {

/// Top-level game application class.
/// sokol drives the main loop; this class holds all game state
/// and is called from the static sapp callbacks.
class Game {
public:
    Game();
    ~Game();

    /// Initialise subsystems, sg_setup, pass action.
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
    sg_pass_action m_pass_action{};

    // Timing
    uint64_t m_last_tick{0};

    static Game* s_instance;
};

} // namespace papaya

#endif // PAPAYA_CORE_GAME_HPP
