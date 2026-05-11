#include "graphics/Mesh.hpp"
#include "core/Log.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace papaya {

// ── Internal helpers ────────────────────────────────────────────

static void build_ring(std::vector<Vertex>& verts, f32 y, f32 r, int sectors)
{
    for (int i = 0; i <= sectors; ++i) {
        f32 a = glm::two_pi<f32>() * static_cast<f32>(i) / static_cast<f32>(sectors);
        verts.push_back({r * std::cos(a), y, r * std::sin(a)});
    }
}

static void build_hemisphere(std::vector<Vertex>& verts,
                             std::vector<u32>& indices,
                             f32 r, int sectors, int stacks, bool top)
{
    const f32 y_base = top ? r : -r;
    const f32 y_sign = top ? 1.0f : -1.0f;
    const int base_idx = static_cast<int>(verts.size());

    // Apex
    verts.push_back({0.0f, y_base + r * y_sign, 0.0f});

    // Stacks (latitude rings from apex to equator)
    for (int j = 1; j <= stacks; ++j) {
        f32 lat = glm::half_pi<f32>() * static_cast<f32>(j) / static_cast<f32>(stacks);
        f32 y   = y_sign * r * std::sin(lat);
        f32 rad = r * std::cos(lat);
        build_ring(verts, y_base + y, rad, sectors);
    }

    const int apex_idx = base_idx;
    const int equator_start = base_idx + 1; // first ring after apex
    const int equator_end   = base_idx + 1 + stacks * (sectors + 1);

    // Triangles: apex to first ring
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(apex_idx);
        indices.push_back(equator_start + i);
        indices.push_back(equator_start + i + 1);
    }

    // Rings between stacks
    for (int j = 0; j < stacks - 1; ++j) {
        int row0 = base_idx + 1 + j * (sectors + 1);
        int row1 = row0 + (sectors + 1);
        for (int i = 0; i < sectors; ++i) {
            indices.push_back(row0 + i);
            indices.push_back(row1 + i);
            indices.push_back(row0 + i + 1);

            indices.push_back(row0 + i + 1);
            indices.push_back(row1 + i);
            indices.push_back(row1 + i + 1);
        }
    }
}

// ── Public API ──────────────────────────────────────────────────

Mesh create_capsule(f32 radius, f32 height, int sectors)
{
    if (height < 2.0f * radius) {
        height = 2.0f * radius;
    }

    const f32 cyl_height = height - 2.0f * radius;
    const f32 half_cyl   = cyl_height * 0.5f;
    const int stacks     = 4; // latitude divisions per hemisphere

    std::vector<Vertex> verts;
    std::vector<u32> indices;

    // ── Bottom hemisphere ──────────────────────────────────────
    build_hemisphere(verts, indices, radius, sectors, stacks, false);

    // ── Cylinder body ──────────────────────────────────────────
    const int cyl_base = static_cast<int>(verts.size());
    // Bottom ring
    build_ring(verts, -half_cyl, radius, sectors);
    // Top ring
    build_ring(verts,  half_cyl, radius, sectors);

    const int bottom_ring = cyl_base;
    const int top_ring    = cyl_base + (sectors + 1);
    for (int i = 0; i < sectors; ++i) {
        indices.push_back(bottom_ring + i);
        indices.push_back(top_ring + i);
        indices.push_back(bottom_ring + i + 1);

        indices.push_back(bottom_ring + i + 1);
        indices.push_back(top_ring + i);
        indices.push_back(top_ring + i + 1);
    }

    // ── Top hemisphere ─────────────────────────────────────────
    build_hemisphere(verts, indices, radius, sectors, stacks, true);

    // ── Upload to GPU ──────────────────────────────────────────
    Mesh mesh;
    mesh.num_vertices = static_cast<int>(verts.size());
    mesh.num_indices   = static_cast<int>(indices.size());
    mesh.prim_type     = SG_PRIMITIVETYPE_TRIANGLES;

    sg_buffer_desc vbd{};
    vbd.data.ptr = verts.data();
    vbd.data.size = verts.size() * sizeof(Vertex);
    mesh.vbuf = sg_make_buffer(&vbd);

    sg_buffer_desc ibd{};
    ibd.data.ptr = indices.data();
    ibd.data.size = indices.size() * sizeof(u32);
    ibd.usage.index_buffer = true;
    mesh.ibuf = sg_make_buffer(&ibd);

    PAPAYA_DEBUG("Capsule: {} verts, {} indices", mesh.num_vertices, mesh.num_indices);
    return mesh;
}

Mesh create_grid(f32 size, int divisions)
{
    const f32 half = size * 0.5f;
    const f32 step = size / static_cast<f32>(divisions);

    std::vector<Vertex> verts;
    std::vector<u32> indices;

    // Lines along X
    for (int i = 0; i <= divisions; ++i) {
        f32 z = -half + static_cast<f32>(i) * step;
        int base = static_cast<int>(verts.size());
        verts.push_back({-half, 0.0f, z});
        verts.push_back({ half, 0.0f, z});
        indices.push_back(base);
        indices.push_back(base + 1);
    }

    // Lines along Z
    for (int i = 0; i <= divisions; ++i) {
        f32 x = -half + static_cast<f32>(i) * step;
        int base = static_cast<int>(verts.size());
        verts.push_back({x, 0.0f, -half});
        verts.push_back({x, 0.0f,  half});
        indices.push_back(base);
        indices.push_back(base + 1);
    }

    Mesh mesh;
    mesh.num_vertices = static_cast<int>(verts.size());
    mesh.num_indices   = static_cast<int>(indices.size());
    mesh.prim_type     = SG_PRIMITIVETYPE_LINES;

    sg_buffer_desc vbd{};
    vbd.data.ptr = verts.data();
    vbd.data.size = verts.size() * sizeof(Vertex);
    mesh.vbuf = sg_make_buffer(&vbd);

    sg_buffer_desc ibd{};
    ibd.data.ptr = indices.data();
    ibd.data.size = indices.size() * sizeof(u32);
    ibd.usage.index_buffer = true;
    mesh.ibuf = sg_make_buffer(&ibd);

    PAPAYA_DEBUG("Grid: {} verts, {} indices", mesh.num_vertices, mesh.num_indices);
    return mesh;
}

Mesh create_box(const Vec3& size)
{
    const f32 hx = size.x * 0.5f;
    const f32 hy = size.y * 0.5f;
    const f32 hz = size.z * 0.5f;

    std::vector<Vertex> verts{
        {-hx, -hy, -hz},
        { hx, -hy, -hz},
        { hx,  hy, -hz},
        {-hx,  hy, -hz},
        {-hx, -hy,  hz},
        { hx, -hy,  hz},
        { hx,  hy,  hz},
        {-hx,  hy,  hz},
    };

    std::vector<u32> indices{
        0, 2, 1, 0, 3, 2,
        4, 5, 6, 4, 6, 7,
        0, 4, 7, 0, 7, 3,
        1, 2, 6, 1, 6, 5,
        3, 7, 6, 3, 6, 2,
        0, 1, 5, 0, 5, 4,
    };

    Mesh mesh;
    mesh.num_vertices = static_cast<int>(verts.size());
    mesh.num_indices = static_cast<int>(indices.size());
    mesh.prim_type = SG_PRIMITIVETYPE_TRIANGLES;

    sg_buffer_desc vbd{};
    vbd.data.ptr = verts.data();
    vbd.data.size = verts.size() * sizeof(Vertex);
    mesh.vbuf = sg_make_buffer(&vbd);

    sg_buffer_desc ibd{};
    ibd.data.ptr = indices.data();
    ibd.data.size = indices.size() * sizeof(u32);
    ibd.usage.index_buffer = true;
    mesh.ibuf = sg_make_buffer(&ibd);

    PAPAYA_DEBUG("Box: {} verts, {} indices", mesh.num_vertices, mesh.num_indices);
    return mesh;
}

void destroy_mesh(Mesh& mesh)
{
    if (mesh.vbuf.id != SG_INVALID_ID) sg_destroy_buffer(mesh.vbuf);
    if (mesh.ibuf.id != SG_INVALID_ID) sg_destroy_buffer(mesh.ibuf);
    mesh = {};
}

} // namespace papaya
