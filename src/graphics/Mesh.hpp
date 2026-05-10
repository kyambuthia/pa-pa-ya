#ifndef PAPAYA_GRAPHICS_MESH_HPP
#define PAPAYA_GRAPHICS_MESH_HPP

#include "core/Types.hpp"

#include <sokol_gfx.h>

namespace papaya {

/// Simple vertex with position only (extend later with normal/uv).
struct Vertex {
    float x, y, z;
};

/// GPU mesh holding sokol buffer handles.
struct Mesh {
    sg_buffer vbuf{};
    sg_buffer ibuf{};
    int       num_vertices{0};
    int       num_indices{0};
    sg_primitive_type prim_type{SG_PRIMITIVETYPE_TRIANGLES};
};

// ── Mesh generators ────────────────────────────────────────────

/// Capsule (cylinder + hemisphere caps) centered at origin.
/// \param radius  Capsule radius
/// \param height  Full height (cylinder + 2 caps)
/// \param sectors Radial subdivisions
Mesh create_capsule(f32 radius, f32 height, int sectors = 16);

/// Flat grid on the XZ plane.
/// \param size      Side length
/// \param divisions Cells per side
Mesh create_grid(f32 size, int divisions = 20);

/// Free GPU resources for a mesh.
void destroy_mesh(Mesh& mesh);

} // namespace papaya

#endif // PAPAYA_GRAPHICS_MESH_HPP
