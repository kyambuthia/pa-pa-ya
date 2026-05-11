#ifndef PAPAYA_GRAPHICS_RENDERER_HPP
#define PAPAYA_GRAPHICS_RENDERER_HPP

#include "gameplay/Player.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "world/World.hpp"

#include <sokol_gfx.h>

namespace papaya {

/// Owns render pipelines, shaders, and drawing.
class Renderer {
public:
    Renderer();

    bool init();
    void shutdown();

    void draw(const Camera& camera, const World& world, const Player& player);

private:
    bool create_shader();
    bool create_pipelines();
    bool create_player_mesh();

    sg_pass_action m_pass_action{};
    sg_shader      m_shader{};
    sg_pipeline    m_pip_triangles{};
    sg_pipeline    m_pip_lines{};
    Mesh           m_player_mesh;
};

} // namespace papaya

#endif // PAPAYA_GRAPHICS_RENDERER_HPP
