#ifndef PAPAYA_WORLD_WORLD_HPP
#define PAPAYA_WORLD_WORLD_HPP

#include "graphics/Mesh.hpp"

namespace papaya {

/// Current world state and static scene content.
class World {
public:
    bool init();
    void shutdown();

    const Mesh& grid() const { return m_grid; }

private:
    Mesh m_grid;
};

} // namespace papaya

#endif // PAPAYA_WORLD_WORLD_HPP
