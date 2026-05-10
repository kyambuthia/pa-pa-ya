/**
 * PaPaYa — A C++ game built with OpenGL via sokol.
 *
 * Entry point.  Hands control to sokol's sapp_run() which drives
 * the application lifecycle through callbacks.
 */

#include "core/Game.hpp"
#include "core/Log.hpp"

#include <sokol_app.h>
#include <sokol_log.h>

// ── sapp callbacks (static, forward to Game singleton) ────────

static void init_cb()
{
    papaya::init_logging();
    if (!papaya::Game::instance()->init()) {
        PAPAYA_CRITICAL("Game initialisation failed — exiting");
        sapp_request_quit();
    }
}

static void frame_cb()
{
    papaya::Game::instance()->update_frame();
}

static void cleanup_cb()
{
    papaya::Game::instance()->shutdown();
    PAPAYA_INFO("Goodbye!");
}

static void event_cb(const sapp_event* e)
{
    papaya::Game::instance()->handle_event(e);
}

// ── entry point ───────────────────────────────────────────────

int main()
{
    // Game must outlive sapp_run because cleanup_cb runs before
    // the Game destructor.
    papaya::Game game;

    sapp_desc desc{};
    desc.init_cb    = init_cb;
    desc.frame_cb   = frame_cb;
    desc.cleanup_cb = cleanup_cb;
    desc.event_cb   = event_cb;
    desc.width      = 1280;
    desc.height     = 720;
    desc.window_title = "PaPaYa";
    desc.logger.func = slog_func;

    // Request OpenGL 3.3 core profile (Intel HD 3000 max).
    desc.gl.major_version = 3;
    desc.gl.minor_version = 3;

    sapp_run(&desc);

    return 0;
}
