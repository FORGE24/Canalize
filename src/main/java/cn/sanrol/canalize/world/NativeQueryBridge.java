package cn.sanrol.canalize.world;

import cn.sanrol.canalize.Canalize;

import java.util.LinkedHashMap;
import java.util.Map;

/**
 * JNI bridge for querying the native library's status at runtime.
 *
 * All public helper methods are safe regardless of whether the native library
 * is loaded — they return an error entry in the map when it is not.
 *
 * JNI naming convention (methods are static, class = NativeQueryBridge):
 *   Java_cn_sanrol_canalize_world_NativeQueryBridge_<methodName>
 */
public class NativeQueryBridge {

    // ------------------------------------------------------------------
    // Raw native declarations (static, called via jni.cpp exports)
    // ------------------------------------------------------------------

    /** Returns pipe-separated status: "version=...|chunks=...|avg_ns=...|..." */
    public static native String nativeGetStatus();

    /** Returns pipe-separated terrain info at block (x, z). */
    public static native String nativeGetTerrainInfo(int blockX, int blockZ);

    /** Resets all chunk generation counters. */
    public static native void nativeResetStats();

    // Individual accessors (available for external querying if needed)
    public static native long   nativeGetChunksGenerated();
    public static native long   nativeGetAvgGenTimeNs();
    public static native long   nativeGetMinGenTimeNs();
    public static native long   nativeGetMaxGenTimeNs();
    public static native String nativeGetLibVersion();

    // ------------------------------------------------------------------
    // Safe wrappers — guard against unloaded library
    // ------------------------------------------------------------------

    /**
     * Returns the full native status as a key-value map.
     * Keys: version, chunks, avg_ns, min_ns, max_ns, total_ms, last_cx, last_cz
     */
    public static Map<String, String> getStatusMap() {
        if (!Canalize.NATIVE_LOADED) {
            return errorMap("native library not loaded");
        }
        try {
            return parsePipeKv(nativeGetStatus());
        } catch (UnsatisfiedLinkError e) {
            return errorMap("UnsatisfiedLinkError: " + e.getMessage());
        }
    }

    /**
     * Returns terrain info for the given block coordinates as a key-value map.
     * Keys: height, biome, biomeId, seaLevel, minY, maxY
     */
    public static Map<String, String> getTerrainInfoMap(int blockX, int blockZ) {
        if (!Canalize.NATIVE_LOADED) {
            return errorMap("native library not loaded");
        }
        try {
            return parsePipeKv(nativeGetTerrainInfo(blockX, blockZ));
        } catch (UnsatisfiedLinkError e) {
            return errorMap("UnsatisfiedLinkError: " + e.getMessage());
        }
    }

    /** Resets all chunk-generation stats (no-op if native not loaded). */
    public static void resetStats() {
        if (!Canalize.NATIVE_LOADED) return;
        try {
            nativeResetStats();
        } catch (UnsatisfiedLinkError ignored) {}
    }

    // ------------------------------------------------------------------
    // Native log drain
    // ------------------------------------------------------------------

    /** Raw JNI: returns "\n"-separated "LEVEL|msg" entries, or "". */
    public static native String nativeDrainLog();

    /** Raw JNI: enable or disable the native log queue. */
    public static native void nativeSetLogEnabled(boolean enabled);

    /** Raw JNI: whether the native log queue is enabled. */
    public static native boolean nativeIsLogEnabled();

    /** Raw JNI: drop all queued log entries without returning them. */
    public static native void nativeClearLog();

    /**
     * Enable or disable native chat debug logging.
     * When enabled, C++ log calls are queued and drained every client tick
     * to the player's chat box.
     */
    public static void setLogEnabled(boolean enabled) {
        if (!Canalize.NATIVE_LOADED) return;
        try {
            nativeSetLogEnabled(enabled);
        } catch (UnsatisfiedLinkError ignored) {}
    }

    /**
     * Drain all pending native log entries.
     * Returns an array of "LEVEL|message" strings (empty array if none).
     * Safe to call even when native is not loaded.
     */
    public static String[] drainLog() {
        if (!Canalize.NATIVE_LOADED) return new String[0];
        try {
            String raw = nativeDrainLog();
            if (raw == null || raw.isEmpty()) return new String[0];
            return raw.split("\\n");
        } catch (UnsatisfiedLinkError e) {
            return new String[]{"ERROR|NativeQueryBridge.drainLog UnsatisfiedLinkError: " + e.getMessage()};
        }
    }

    // ------------------------------------------------------------------
    // Formatting helpers
    // ------------------------------------------------------------------

    /**
     * Converts a nanosecond value to a human-readable string:
     *   < 1 µs  → "X ns"
     *   < 1 ms  → "X.XX µs"
     *   else    → "X.XX ms"
     */
    public static String formatNs(long ns) {
        if (ns <= 0)       return "N/A";
        if (ns < 1_000L)   return ns + " ns";
        if (ns < 1_000_000L) return String.format("%.2f µs", ns / 1_000.0);
        return String.format("%.2f ms", ns / 1_000_000.0);
    }

    // ------------------------------------------------------------------
    // Internal helpers
    // ------------------------------------------------------------------

    private static Map<String, String> parsePipeKv(String s) {
        Map<String, String> map = new LinkedHashMap<>();
        if (s == null || s.isEmpty()) return map;
        for (String part : s.split("\\|")) {
            int eq = part.indexOf('=');
            if (eq > 0) map.put(part.substring(0, eq), part.substring(eq + 1));
        }
        return map;
    }

    private static Map<String, String> errorMap(String msg) {
        Map<String, String> m = new LinkedHashMap<>();
        m.put("error", msg);
        return m;
    }
}
