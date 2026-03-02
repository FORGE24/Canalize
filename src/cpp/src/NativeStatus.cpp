#include "../include/NativeStatus.h"
#include <climits>
#include <sstream>
#include <iomanip>

namespace NativeStatus {

    // -----------------------------------------------------------------------
    // Storage
    // -----------------------------------------------------------------------
    std::atomic<int64_t> chunksGenerated{0};
    std::atomic<int64_t> totalGenTimeNs{0};
    std::atomic<int64_t> minGenTimeNs{INT64_MAX};
    std::atomic<int64_t> maxGenTimeNs{0};
    std::atomic<int32_t> lastChunkX{0};
    std::atomic<int32_t> lastChunkZ{0};

    // -----------------------------------------------------------------------
    // recordChunkGen — call after every generate_chunk()
    // All CAS loops are wait-free on uncontended paths (typical for chunks).
    // -----------------------------------------------------------------------
    void recordChunkGen(int chunkX, int chunkZ, int64_t durationNs) {
        chunksGenerated.fetch_add(1, std::memory_order_relaxed);
        totalGenTimeNs.fetch_add(durationNs, std::memory_order_relaxed);

        // Update minimum (lock-free CAS)
        int64_t cur_min = minGenTimeNs.load(std::memory_order_relaxed);
        while (durationNs < cur_min) {
            if (minGenTimeNs.compare_exchange_weak(
                    cur_min, durationNs,
                    std::memory_order_relaxed, std::memory_order_relaxed))
                break;
        }

        // Update maximum (lock-free CAS)
        int64_t cur_max = maxGenTimeNs.load(std::memory_order_relaxed);
        while (durationNs > cur_max) {
            if (maxGenTimeNs.compare_exchange_weak(
                    cur_max, durationNs,
                    std::memory_order_relaxed, std::memory_order_relaxed))
                break;
        }

        lastChunkX.store(chunkX, std::memory_order_relaxed);
        lastChunkZ.store(chunkZ, std::memory_order_relaxed);
    }

    // -----------------------------------------------------------------------
    // reset — zero all counters
    // -----------------------------------------------------------------------
    void reset() {
        chunksGenerated.store(0, std::memory_order_relaxed);
        totalGenTimeNs.store(0, std::memory_order_relaxed);
        minGenTimeNs.store(INT64_MAX, std::memory_order_relaxed);
        maxGenTimeNs.store(0, std::memory_order_relaxed);
        lastChunkX.store(0, std::memory_order_relaxed);
        lastChunkZ.store(0, std::memory_order_relaxed);
    }

    // -----------------------------------------------------------------------
    // buildStatusString — multi-line formatted summary
    // -----------------------------------------------------------------------
    std::string buildStatusString() {
        int64_t count   = chunksGenerated.load(std::memory_order_relaxed);
        int64_t total   = totalGenTimeNs.load(std::memory_order_relaxed);
        int64_t minT    = minGenTimeNs.load(std::memory_order_relaxed);
        int64_t maxT    = maxGenTimeNs.load(std::memory_order_relaxed);
        int64_t avg     = (count > 0) ? (total / count) : 0;
        int32_t lx      = lastChunkX.load(std::memory_order_relaxed);
        int32_t lz      = lastChunkZ.load(std::memory_order_relaxed);

        // Use pipe separators so Java side can split easily
        std::ostringstream oss;
        oss << "version="   << LIB_VERSION
            << "|chunks="   << count
            << "|avg_ns="   << avg
            << "|min_ns="   << (count > 0 ? minT : 0LL)
            << "|max_ns="   << maxT
            << "|total_ms=" << (total / 1000000LL)
            << "|last_cx="  << lx
            << "|last_cz="  << lz;
        return oss.str();
    }

} // namespace NativeStatus
