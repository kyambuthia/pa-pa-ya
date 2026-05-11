#include "graphics/Renderer.hpp"

#include "core/Log.hpp"

#include <fstream>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <sokol_glue.h>

namespace papaya {

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

Renderer::Renderer()
{
    m_pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    m_pass_action.colors[0].clear_value = {0.12f, 0.12f, 0.16f, 1.0f};
}

bool Renderer::init()
{
    if (!create_shader()) return false;
    if (!create_pipelines()) return false;
    if (!create_player_meshes()) return false;

    return true;
}

bool Renderer::create_shader()
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
    sd.uniform_blocks[0].size = sizeof(Mat4);
    sd.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_NATIVE;
    sd.uniform_blocks[0].glsl_uniforms[0].glsl_name = "u_mvp";
    sd.uniform_blocks[0].glsl_uniforms[0].type = SG_UNIFORMTYPE_MAT4;

    sd.fragment_func.source = fs_src.c_str();
    sd.fragment_func.entry  = "main";

    sd.uniform_blocks[1].stage = SG_SHADERSTAGE_FRAGMENT;
    sd.uniform_blocks[1].size = sizeof(Vec4);
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

bool Renderer::create_pipelines()
{
    sg_pipeline_desc pd{};

    pd.layout.buffers[0].stride = sizeof(Vertex);
    pd.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    pd.layout.attrs[0].buffer_index = 0;
    pd.layout.attrs[0].offset = 0;

    pd.depth.write_enabled = true;
    pd.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pd.face_winding = SG_FACEWINDING_CCW;

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

bool Renderer::create_player_meshes()
{
    m_walker_mesh = create_capsule(0.5f, 2.0f, 16);
    if (m_walker_mesh.vbuf.id == SG_INVALID_ID || m_walker_mesh.ibuf.id == SG_INVALID_ID) {
        PAPAYA_ERROR("Failed to create walker capsule mesh");
        return false;
    }

    m_vehicle_mesh = create_box(Vec3{1.8f, 0.8f, 2.6f});
    if (m_vehicle_mesh.vbuf.id == SG_INVALID_ID || m_vehicle_mesh.ibuf.id == SG_INVALID_ID) {
        PAPAYA_ERROR("Failed to create vehicle box mesh");
        return false;
    }

    return true;
}

void Renderer::draw(const Camera& camera, const World& world, const Player& player)
{
    sg_pass pass{};
    pass.action = m_pass_action;
    pass.swapchain = sglue_swapchain();
    sg_begin_pass(&pass);

    Mat4 vp = camera.view_projection();

    sg_bindings bind{};
    bind.vertex_buffers[0] = world.grid().vbuf;
    bind.index_buffer = world.grid().ibuf;

    sg_apply_pipeline(m_pip_lines);
    sg_apply_bindings(&bind);

    Vec4 grid_color{0.3f, 0.3f, 0.35f, 1.0f};
    sg_apply_uniforms(0, SG_RANGE(vp));
    sg_apply_uniforms(1, SG_RANGE(grid_color));
    sg_draw(0, world.grid().num_indices, 1);

    const f32 vehicle_blend = player.vehicle_blend();

    auto draw_form = [&](const Mesh& mesh, const Mat4& model, const Vec4& color) {
        Mat4 mvp = vp * model;
        bind.vertex_buffers[0] = mesh.vbuf;
        bind.index_buffer = mesh.ibuf;

        sg_apply_pipeline(m_pip_triangles);
        sg_apply_bindings(&bind);
        sg_apply_uniforms(0, SG_RANGE(mvp));
        sg_apply_uniforms(1, SG_RANGE(color));
        sg_draw(0, mesh.num_indices, 1);
    };

    if (vehicle_blend < 0.99f) {
        const f32 walker_scale = (vehicle_blend <= 0.01f) ? 1.0f : glm::mix(1.0f, 0.35f, vehicle_blend);
        Mat4 walker_model = player.render_matrix(PlayerMode::Walker);
        walker_model = walker_model * glm::scale(Mat4{1.0f}, Vec3{walker_scale, walker_scale, walker_scale});
        draw_form(m_walker_mesh, walker_model, Vec4{0.9f, 0.85f, 0.7f, 1.0f});
    }

    if (vehicle_blend > 0.01f) {
        const f32 vehicle_scale = (vehicle_blend >= 0.99f) ? 1.0f : glm::mix(0.35f, 1.0f, vehicle_blend);
        Mat4 vehicle_model = player.render_matrix(PlayerMode::Vehicle);
        vehicle_model = vehicle_model * glm::scale(Mat4{1.0f}, Vec3{vehicle_scale, vehicle_scale, vehicle_scale});
        draw_form(m_vehicle_mesh, vehicle_model, Vec4{0.35f, 0.65f, 0.95f, 1.0f});
    }

    sg_end_pass();
    sg_commit();
}

void Renderer::shutdown()
{
    destroy_mesh(m_walker_mesh);
    destroy_mesh(m_vehicle_mesh);

    if (m_pip_triangles.id != SG_INVALID_ID) sg_destroy_pipeline(m_pip_triangles);
    if (m_pip_lines.id != SG_INVALID_ID)     sg_destroy_pipeline(m_pip_lines);
    if (m_shader.id != SG_INVALID_ID)        sg_destroy_shader(m_shader);
}

} // namespace papaya
