#include "world/World.hpp"

#include "core/Log.hpp"

namespace papaya {

bool World::init()
{
    m_grid = create_grid(20.0f, 20);
    if (m_grid.vbuf.id == SG_INVALID_ID || m_grid.ibuf.id == SG_INVALID_ID) {
        PAPAYA_ERROR("Failed to create grid mesh");
        return false;
    }

    return true;
}

void World::shutdown()
{
    destroy_mesh(m_grid);
}

} // namespace papaya
