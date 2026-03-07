#pragma once
#include "CanalizeAPI.h"
#include <string>
#include <deque>
#include <mutex>
#include <cstdint>

// ===========================================================================
// NativeLog — thread-safe in-game chat log queue.
// ===========================================================================
namespace NativeLog {

    enum class Level : uint8_t {
        DEBUG = 0,
        INFO  = 1,
        WARN  = 2,
        ERROR = 3
    };

    constexpr size_t MAX_QUEUE = 512;

    // -----------------------------------------------------------------------
    // Enqueue a message. Thread-safe (mutex-protected).
    // -----------------------------------------------------------------------
    CANALIZE_API void log(Level level, const std::string& msg);

    // Convenience overloads
    CANALIZE_API inline void debug(const std::string& msg) { log(Level::DEBUG, msg); }
    CANALIZE_API inline void info (const std::string& msg) { log(Level::INFO,  msg); }
    CANALIZE_API inline void warn (const std::string& msg) { log(Level::WARN,  msg); }
    CANALIZE_API inline void error(const std::string& msg) { log(Level::ERROR, msg); }

    // -----------------------------------------------------------------------
    // Drain all pending messages. Thread-safe.
    // -----------------------------------------------------------------------
    CANALIZE_API std::string drain();

    // -----------------------------------------------------------------------
    // Return whether the queue currently has any messages.
    // -----------------------------------------------------------------------
    CANALIZE_API bool has_pending();

    // -----------------------------------------------------------------------
    // Drop all queued messages without returning them.
    // -----------------------------------------------------------------------
    CANALIZE_API void clear();

    // -----------------------------------------------------------------------
    // Runtime enable/disable
    // -----------------------------------------------------------------------
    CANALIZE_API void set_enabled(bool enabled);
    CANALIZE_API bool is_enabled();

} // namespace NativeLog
