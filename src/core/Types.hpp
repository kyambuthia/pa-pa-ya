#ifndef PAPAYA_CORE_TYPES_HPP
#define PAPAYA_CORE_TYPES_HPP

#include <cstdint>
#include <glm/glm.hpp>

namespace papaya {

// ── Integer aliases ───────────────────────────────────────────
using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

// ── Common math aliases ───────────────────────────────────────
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;

using Mat4 = glm::mat4;
using Mat3 = glm::mat3;

// ── Colour ────────────────────────────────────────────────────
struct Colour {
    f32 r{1.0f};
    f32 g{1.0f};
    f32 b{1.0f};
    f32 a{1.0f};

    static constexpr Colour Black()   { return {0.0f, 0.0f, 0.0f, 1.0f}; }
    static constexpr Colour White()   { return {1.0f, 1.0f, 1.0f, 1.0f}; }
    static constexpr Colour Red()     { return {1.0f, 0.0f, 0.0f, 1.0f}; }
    static constexpr Colour Green()   { return {0.0f, 1.0f, 0.0f, 1.0f}; }
    static constexpr Colour Blue()    { return {0.0f, 0.0f, 1.0f, 1.0f}; }
    static constexpr Colour Cyan()    { return {0.0f, 1.0f, 1.0f, 1.0f}; }
    static constexpr Colour Yellow()  { return {1.0f, 1.0f, 0.0f, 1.0f}; }
    static constexpr Colour Magenta() { return {1.0f, 0.0f, 1.0f, 1.0f}; }
};

} // namespace papaya

#endif // PAPAYA_CORE_TYPES_HPP
