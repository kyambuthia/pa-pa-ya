#ifndef PAPAYA_CORE_LOG_HPP
#define PAPAYA_CORE_LOG_HPP

/// Convenience macros wrapping spdlog.
/// Logs carry the source file and line for easy debugging.

#include <spdlog/spdlog.h>
#include <cassert>

// These forward to spdlog's default logger.
// In a more advanced setup you'd create named loggers per system.

#define PAPAYA_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define PAPAYA_DEBUG(...)    SPDLOG_DEBUG(__VA_ARGS__)
#define PAPAYA_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define PAPAYA_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define PAPAYA_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
#define PAPAYA_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

/// Assert that fires PAPAYA_CRITICAL before aborting.
#define PAPAYA_ASSERT(cond, msg)                       \
    do {                                                \
        if (!(cond)) {                                  \
            PAPAYA_CRITICAL("ASSERT: {} -- {}", msg, #cond); \
            assert(cond);                                \
        }                                               \
    } while (false)

namespace papaya {

/// Initialise the logging system. Call once at startup.
inline void init_logging()
{
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%T] [%^%l%$] %v");
}

} // namespace papaya

#endif // PAPAYA_CORE_LOG_HPP
