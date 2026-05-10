#include "core/Game.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace papaya {

// ── File I/O helper ─────────────────────────────────────────────

static std::string read_file(const char* path)
{
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        PAPAYA_ERROR("Failed to open file: {}", path);
        return {};
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// ── Statics ─────────────────────────────────────────────────────

Game* Game::s_instance = nullptr;

// ── Constructor / Destructor ────────────────────────────────────

Game::Game()
{
    PAPAYA_ASSERT(s_instance == nullptr, "Only one Game instance allowed");
    s_instance = this;

    m_pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    m_pass_action.colors[0].clear_value = { 0.12f, 0.12f, 0.16f, 1.0f };
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

    // ── shader ───────────────────────────────────────────────
    if (!create_shader()) return false;

    // ── pipelines ────────────────────────────────────────────
    if (!create_pipelines()) return false;

    // ── scene meshes ─────────────────────────────────────────
    if (!create_scene_meshes()) return false;

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

bool Game::create_shader()
{
    std::string vs_src = read_file("assets/shaders/unlit_color.vert");
    std::string fs_src = read_file("assets/shaders/unlit_color.frag");
    if (vs_src.empty() || fs_src.empty()) {
        PAPAYA_ERROR("Failed to load shader source files");
        return false;
    }

    sg_shader_desc sd{};

    sd.vertex_func.source = vs_src.c_str();
    sd.vertex_func.entry  = "main";

    sd.uniform_blocks[0].stage = SG_SHADERSTAGE_VERTEX;
    sd.uniform_blocks[0].size  = sizeof(Mat4);
    sd.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_NATIVE;
    sd.uniform_blocks[0].glsl_uniforms[0].glsl_name = "u_mvp";
    sd.uniform_blocks[0].glsl_uniforms[0].type = SG_UNIFORMTYPE_MAT4;

    sd.fragment_func.source = fs_src.c_str();
    sd.fragment_func.entry  = "main";

    sd.uniform_blocks[1].stage = SG_SHADERSTAGE_FRAGMENT;
    sd.uniform_blocks[1].size  = sizeof(Vec4);
    sd.uniform_blocks[1].layout = SG_UNIFORMLAYOUT_NATIVE;
    sd.uniform_blocks[1].glsl_uniforms[0].glsl_name = "u_color";
    sd.uniform_blocks[1].glsl_uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;

    sd.attrs[0].glsl_name = "a_pos";
    sd.attrs[0].base_type = SG_SHADERATTRBASETYPE_FLOAT;

    m_shader = sg_make_shader(&sd);
    if (m_shader.id == SG_INVALID_ID) {
        PAPAYA_ERROR("sg_make_shader failed");
        return false;
    }
    PAPAYA_DEBUG("Shader created successfully");
    return true;
}

bool Game::create_pipelines()
{
    sg_pipeline_desc pd{};

    pd.layout.buffers[0].stride = sizeof(Vertex);
    pd.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    pd.layout.attrs[0].buffer_index = 0;
    pd.layout.attrs[0].offset = 0;

    pd.depth.write_enabled = true;
    pd.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pd.face_winding = SG_FACEWINDING_CCW;

    // ── Triangles pipeline ───────────────────────────────────
    pd.shader = m_shader;
    pd.primitive_type = SG_PRIMITIVETYPE_TRIANGLES;
    pd.index_type = SG_INDEXTYPE_UINT32;
    pd.cull_mode = SG_CULLMODE_BACK;
    pd.label = "triangles-pipeline";
    m_pip_triangles = sg_make_pipeline(&pd);
    if (m_pip_triangles.id == SG_INVALID_ID) {
        PAPAYA_ERROR("Failed to create triangle pipeline");
        return false;
    }

    // ── Lines pipeline ───────────────────────────────────────
    pd.primitive_type = SG_PRIMITIVETYPE_LINES;
    pd.cull_mode = SG_CULLMODE_NONE;
    pd.label = "lines-pipeline";
    m_pip_lines = sg_make_pipeline(&pd);
    if (m_pip_lines.id == SG_INVALID_ID) {
        PAPAYA_ERROR("Failed to create line pipeline");
        return false;
    }

    PAPAYA_DEBUG("Pipelines created successfully");
    return true;
}

bool Game::create_scene_meshes()
{
    m_capsule = create_capsule(0.5f, 2.0f, 16);
    if (m_capsule.vbuf.id == SG_INVALID_ID) {
        PAPAYA_ERROR("Failed to create capsule mesh");
        return false;
    }

    m_grid = create_grid(20.0f, 20);
    if (m_grid.vbuf.id == SG_INVALID_ID) {
        PAPAYA_ERROR("Failed to create grid mesh");
        return false;
    }

    return true;
}

// ── Frame update ────────────────────────────────────────────────

void Game::update_frame()
{
    // ── delta time ───────────────────────────────────────────
    uint64_t now = stm_now();
    f32 dt = static_cast<f32>(stm_sec(now - m_last_tick));
    m_last_tick = now;

    // ── player movement (relative to camera yaw) ─────────────
    f32 f = 0.0f, r = 0.0f, u = 0.0f;
    if (m_keys_held[SAPP_KEYCODE_W]) f += 1.0f;
    if (m_keys_held[SAPP_KEYCODE_S]) f -= 1.0f;
    if (m_keys_held[SAPP_KEYCODE_A]) r -= 1.0f;
    if (m_keys_held[SAPP_KEYCODE_D]) r += 1.0f;
    if (m_keys_held[SAPP_KEYCODE_SPACE])        u += 1.0f;
    if (m_keys_held[SAPP_KEYCODE_LEFT_SHIFT])   u -= 1.0f;

    if (f != 0.0f || r != 0.0f) {
        Vec3 f_dir{std::sin(m_cam_yaw), 0.0f, std::cos(m_cam_yaw)};
        Vec3 r_dir{std::cos(m_cam_yaw), 0.0f, -std::sin(m_cam_yaw)};
        m_player_pos += (f_dir * f + r_dir * r) * m_player_speed * dt;
    }
    m_player_pos.y += u * m_player_speed * dt;
    if (m_player_pos.y < 0.0f) m_player_pos.y = 0.0f;  // keep above ground

    // ── camera orbit ────────────────────────────────────────
    Vec3 offset;
    offset.x = m_cam_distance * std::sin(m_cam_yaw) * std::cos(m_cam_pitch);
    offset.y = m_cam_distance * std::sin(m_cam_pitch);
    offset.z = m_cam_distance * std::cos(m_cam_yaw) * std::cos(m_cam_pitch);

    Vec3 cam_pos = m_player_pos + offset;
    m_camera.set_view(glm::lookAt(cam_pos, m_player_pos, Vec3{0.0f, 1.0f, 0.0f}));

    // ── draw ─────────────────────────────────────────────────
    sg_pass pass{};
    pass.action = m_pass_action;
    pass.swapchain = sglue_swapchain();
    sg_begin_pass(&pass);

    Mat4 vp = m_camera.view_projection();

    // ── Grid ─────────────────────────────────────────────────
    sg_bindings bind{};
    bind.vertex_buffers[0] = m_grid.vbuf;
    bind.index_buffer = m_grid.ibuf;

    sg_apply_pipeline(m_pip_lines);
    sg_apply_bindings(&bind);

    Vec4 grid_color{0.3f, 0.3f, 0.35f, 1.0f};
    sg_apply_uniforms(0, SG_RANGE(vp));
    sg_apply_uniforms(1, SG_RANGE(grid_color));
    sg_draw(0, m_grid.num_indices, 1);

    // ── Capsule (player) ─────────────────────────────────────
    Mat4 model = glm::translate(Mat4{1.0f}, m_player_pos);
    Mat4 mvp   = vp * model;

    bind.vertex_buffers[0] = m_capsule.vbuf;
    bind.index_buffer = m_capsule.ibuf;

    sg_apply_pipeline(m_pip_triangles);
    sg_apply_bindings(&bind);

    Vec4 capsule_color{0.9f, 0.85f, 0.7f, 1.0f};
    sg_apply_uniforms(0, SG_RANGE(mvp));
    sg_apply_uniforms(1, SG_RANGE(capsule_color));
    sg_draw(0, m_capsule.num_indices, 1);

    sg_end_pass();
    sg_commit();
}

// ── Shutdown ────────────────────────────────────────────────────

void Game::shutdown()
{
    PAPAYA_TRACE("Game::shutdown()");

    destroy_mesh(m_capsule);
    destroy_mesh(m_grid);

    if (m_pip_triangles.id != SG_INVALID_ID) sg_destroy_pipeline(m_pip_triangles);
    if (m_pip_lines.id != SG_INVALID_ID)     sg_destroy_pipeline(m_pip_lines);
    if (m_shader.id != SG_INVALID_ID)         sg_destroy_shader(m_shader);

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
