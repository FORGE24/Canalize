#pragma once
#include <string>
#include <deque>
#include <mutex>
#include <cstdint>

// ===========================================================================
// NativeLog — thread-safe in-game chat log queue.
//
// Any C++ module can call NativeLog::log(level, msg) to enqueue a colored
// message that will be drained by the Java tick loop and displayed in the
// player's chat box in real-time.
//
// Message format passed to Java (one entry per deque element):
//   "<LEVEL>|<message text>"
// Java side maps level -> ChatFormatting color:
//   DEBUG -> GRAY, INFO -> WHITE, WARN -> YELLOW, ERROR -> RED
//
// The queue is bounded (MAX_QUEUE) to avoid unbounded memory growth during
// intensive generation. Oldest entries are dropped when full.
// ===========================================================================
namespace NativeLog {

    enum class Level : uint8_t {
        DEBUG = 0,
        INFO  = 1,
        WARN  = 2,
        ERROR = 3
    };

    // Maximum enqueued messages before oldest entries are dropped.
    constexpr size_t MAX_QUEUE = 512;

    // -----------------------------------------------------------------------
    // Enqueue a message. Thread-safe (mutex-protected).
    // -----------------------------------------------------------------------
    void log(Level level, const std::string& msg);

    // Convenience overloads
    inline void debug(const std::string& msg) { log(Level::DEBUG, msg); }
    inline void info (const std::string& msg) { log(Level::INFO,  msg); }
    inline void warn (const std::string& msg) { log(Level::WARN,  msg); }
    inline void error(const std::string& msg) { log(Level::ERROR, msg); }

    // -----------------------------------------------------------------------
    // Drain all pending messages. Thread-safe.
    // Returns a newline-separated string: "LEVEL|msg\nLEVEL|msg\n..."
    // Returns "" if the queue is empty.
    // -----------------------------------------------------------------------
    std::string drain();

    // -----------------------------------------------------------------------
    // Return whether the queue currently has any messages.
    // -----------------------------------------------------------------------
    bool has_pending();

    // -----------------------------------------------------------------------
    // Drop all queued messages without returning them.
    // -----------------------------------------------------------------------
    void clear();

    // -----------------------------------------------------------------------
    // Runtime enable/disable — when disabled, log() is a no-op.
    // Controlled by /canalize debug on|off via JNI.
    // -----------------------------------------------------------------------
    void set_enabled(bool enabled);
    bool is_enabled();

} // namespace NativeLog
