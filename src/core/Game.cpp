#include "core/Game.hpp"

namespace papaya {

Game* Game::s_instance = nullptr;

Game::Game()
{
    PAPAYA_ASSERT(s_instance == nullptr, "Only one Game instance allowed");
    s_instance = this;

    // Initialise default pass action (cyan clear)
    m_pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    m_pass_action.colors[0].clear_value = { 0.0f, 0.8f, 0.8f, 1.0f };
}

Game::~Game()
{
    s_instance = nullptr;
}

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

    PAPAYA_INFO("Game initialised successfully");
    return true;
}

void Game::update_frame()
{
    // ── delta time ───────────────────────────────────────────
    uint64_t now = stm_now();
    double dt = stm_sec(now - m_last_tick);
    m_last_tick = now;

    // ── draw ─────────────────────────────────────────────────
    sg_pass pass{};
    pass.action = m_pass_action;
    pass.swapchain = sglue_swapchain();
    sg_begin_pass(&pass);

    // TODO: draw calls go here

    sg_end_pass();
    sg_commit();

    (void)dt;
}

void Game::shutdown()
{
    PAPAYA_TRACE("Game::shutdown()");
    sg_shutdown();
}

void Game::handle_event(const sapp_event* e)
{
    switch (e->type) {
        case SAPP_EVENTTYPE_KEY_DOWN:
            if (e->key_code == SAPP_KEYCODE_ESCAPE) {
                sapp_request_quit();
            }
            break;
        default:
            break;
    }
}

} // namespace papaya
