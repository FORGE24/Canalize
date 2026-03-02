#include "../include/NativeLog.h"
#include <atomic>
#include <sstream>

namespace NativeLog {

    // -----------------------------------------------------------------------
    // Internal state
    // -----------------------------------------------------------------------
    static std::mutex       s_mutex;
    static std::deque<std::string> s_queue;
    static std::atomic<bool> s_enabled{false};  // off by default

    static const char* level_tag(Level level) {
        switch (level) {
            case Level::DEBUG: return "DEBUG";
            case Level::INFO:  return "INFO";
            case Level::WARN:  return "WARN";
            case Level::ERROR: return "ERROR";
            default:           return "INFO";
        }
    }

    // -----------------------------------------------------------------------
    // log
    // -----------------------------------------------------------------------
    void log(Level level, const std::string& msg) {
        if (!s_enabled.load(std::memory_order_relaxed)) return;

        std::string entry;
        entry.reserve(msg.size() + 8);
        entry += level_tag(level);
        entry += '|';
        entry += msg;

        std::lock_guard<std::mutex> lock(s_mutex);
        if (s_queue.size() >= MAX_QUEUE) {
            s_queue.pop_front();  // drop oldest
        }
        s_queue.push_back(std::move(entry));
    }

    // -----------------------------------------------------------------------
    // drain — swap-and-return to minimise lock hold time
    // -----------------------------------------------------------------------
    std::string drain() {
        std::deque<std::string> local;
        {
            std::lock_guard<std::mutex> lock(s_mutex);
            local.swap(s_queue);
        }
        if (local.empty()) return "";

        std::ostringstream oss;
        bool first = true;
        for (const auto& entry : local) {
            if (!first) oss << '\n';
            oss << entry;
            first = false;
        }
        return oss.str();
    }

    // -----------------------------------------------------------------------
    // has_pending
    // -----------------------------------------------------------------------
    bool has_pending() {
        std::lock_guard<std::mutex> lock(s_mutex);
        return !s_queue.empty();
    }

    // -----------------------------------------------------------------------
    // clear
    // -----------------------------------------------------------------------
    void clear() {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_queue.clear();
    }

    // -----------------------------------------------------------------------
    // enable / disable
    // -----------------------------------------------------------------------
    void set_enabled(bool enabled) {
        s_enabled.store(enabled, std::memory_order_relaxed);
        if (!enabled) clear();  // discard stale messages on disable
    }

    bool is_enabled() {
        return s_enabled.load(std::memory_order_relaxed);
    }

} // namespace NativeLog
