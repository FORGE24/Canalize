#pragma once
#include <atomic>
#include <cstdint>
#include <string>

// ===========================================================================
// NativeStatus — lock-free statistics tracking for native chunk generation.
// All fields are std::atomic so they can be accessed from multiple threads
// (ForkJoinPool workers) without locks.
// ===========================================================================
namespace NativeStatus {

    // -----------------------------------------------------------------------
    // Build-time version string reported to the /canalize status command.
    // -----------------------------------------------------------------------
    constexpr const char* LIB_VERSION = "1.0.0-avx2";

    // -----------------------------------------------------------------------
    // Counters (updated by WorldLoader::generate_chunk every call)
    // -----------------------------------------------------------------------
    extern std::atomic<int64_t> chunksGenerated;   // total chunks generated
    extern std::atomic<int64_t> totalGenTimeNs;    // cumulative time (ns)
    extern std::atomic<int64_t> minGenTimeNs;      // fastest chunk (ns)
    extern std::atomic<int64_t> maxGenTimeNs;      // slowest chunk (ns)
    extern std::atomic<int32_t> lastChunkX;        // last generated chunk X
    extern std::atomic<int32_t> lastChunkZ;        // last generated chunk Z

    // -----------------------------------------------------------------------
    // Called by WorldLoader::generate_chunk after each chunk is built.
    // Thread-safe.
    // -----------------------------------------------------------------------
    void recordChunkGen(int chunkX, int chunkZ, int64_t durationNs);

    // -----------------------------------------------------------------------
    // Reset all counters to zero (exposed as /canalize reset).
    // -----------------------------------------------------------------------
    void reset();

    // -----------------------------------------------------------------------
    // Helpers
    // -----------------------------------------------------------------------
    inline int64_t getAvgGenTimeNs() {
        int64_t count = chunksGenerated.load(std::memory_order_relaxed);
        if (count == 0) return 0;
        return totalGenTimeNs.load(std::memory_order_relaxed) / count;
    }

    // Returns a multi-line status string (for /canalize status).
    std::string buildStatusString();

} // namespace NativeStatus
