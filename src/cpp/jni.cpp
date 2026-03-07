#include <jni.h>
#include <iostream>
#include <string>
#include "include/WorldLoader.h"
#include "include/NativeStatus.h"
#include "include/NativeLog.h"

// JNI EXPORTS

void perform_hijack()   { std::cout<<"[Native] Hijack Successful!"<<std::endl; }
void load_model_block() { std::cout<<"[Native] MODEL Block Loaded."<<std::endl; }

// Helper: convert std::string -> jstring
static jstring toJString(JNIEnv* env, const std::string& s) {
    return env->NewStringUTF(s.c_str());
}

#include "include/canalize/api/JNIHelper.h"

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){ 
    Canalize::JNIHelper::init(vm); // Initialize Helper
    WorldLoader::init();
    return JNI_VERSION_1_8; 
}
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved){
    Canalize::JNIHelper::detach();
}

JNIEXPORT void JNICALL Java_cn_sanrol_canalize_Canalize_initNative(
        JNIEnv* env, jclass clazz){ perform_hijack(); load_model_block(); }

// ---------------------------------------------------------------------------
// NativeChunkGenerator
// ---------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeChunkGenerator_generateNoiseNative(
        JNIEnv* env, jobject thiz, jobject chunk){}
JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeChunkGenerator_generateSurfaceNative(
        JNIEnv* env, jobject thiz, jobject chunk){}

JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeChunkGenerator_generateChunkData(
        JNIEnv* env, jobject thiz, jint chunkX, jint chunkZ, jintArray blockDataArray){
    jboolean isCopy;
    jint* buffer=env->GetIntArrayElements(blockDataArray,&isCopy);
    if(buffer){
        WorldLoader::generate_chunk(chunkX, chunkZ, (int*)buffer);
        env->ReleaseIntArrayElements(blockDataArray,buffer,0);
    }
}

JNIEXPORT jint JNICALL Java_cn_sanrol_canalize_world_NativeChunkGenerator_getHeightNative(
        JNIEnv* env, jobject thiz, jint x, jint z){
    return WorldLoader::get_height(x,z);
}

// ---------------------------------------------------------------------------
// NativeBiomeSource
// ---------------------------------------------------------------------------
JNIEXPORT jint JNICALL Java_cn_sanrol_canalize_world_NativeBiomeSource_getBiomeNative(
        JNIEnv* env, jobject thiz, jint x, jint z){
    return WorldLoader::get_biome(x,z);
}

// ===========================================================================
// NativeQueryBridge  —  status query commands (/canalize status|terrain|reset)
// ===========================================================================

// nativeGetStatus() -> String
// Returns pipe-separated key=value pairs: version=|chunks=|avg_ns=|...
JNIEXPORT jstring JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeGetStatus(
        JNIEnv* env, jclass clazz){
    return toJString(env, WorldLoader::get_status());
}

// nativeGetTerrainInfo(int blockX, int blockZ) -> String
// Returns: "height=X|biome=NAME|biomeId=N|seaLevel=63|minY=-64|maxY=1743"
JNIEXPORT jstring JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeGetTerrainInfo(
        JNIEnv* env, jclass clazz, jint blockX, jint blockZ){
    return toJString(env, WorldLoader::get_terrain_info((int)blockX, (int)blockZ));
}

// nativeResetStats() -> void
JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeResetStats(
        JNIEnv* env, jclass clazz){
    NativeStatus::reset();
}

// nativeGetChunksGenerated() -> long
JNIEXPORT jlong JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeGetChunksGenerated(
        JNIEnv* env, jclass clazz){
    return (jlong)NativeStatus::chunksGenerated.load(std::memory_order_relaxed);
}

// nativeGetAvgGenTimeNs() -> long
JNIEXPORT jlong JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeGetAvgGenTimeNs(
        JNIEnv* env, jclass clazz){
    return (jlong)NativeStatus::getAvgGenTimeNs();
}

// nativeGetMinGenTimeNs() -> long
JNIEXPORT jlong JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeGetMinGenTimeNs(
        JNIEnv* env, jclass clazz){
    int64_t v = NativeStatus::minGenTimeNs.load(std::memory_order_relaxed);
    return (jlong)(v == INT64_MAX ? 0LL : v); // return 0 if no chunks yet
}

// nativeGetMaxGenTimeNs() -> long
JNIEXPORT jlong JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeGetMaxGenTimeNs(
        JNIEnv* env, jclass clazz){
    return (jlong)NativeStatus::maxGenTimeNs.load(std::memory_order_relaxed);
}

// nativeGetLibVersion() -> String
JNIEXPORT jstring JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeGetLibVersion(
        JNIEnv* env, jclass clazz){
    return toJString(env, NativeStatus::LIB_VERSION);
}

// ===========================================================================
// NativeLog drain / control
// ===========================================================================

// nativeDrainLog() -> String
// Returns newline-separated "LEVEL|message" entries, or "" if empty.
JNIEXPORT jstring JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeDrainLog(
        JNIEnv* env, jclass clazz){
    return toJString(env, NativeLog::drain());
}

// nativeSetLogEnabled(boolean) -> void
JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeSetLogEnabled(
        JNIEnv* env, jclass clazz, jboolean enabled){
    NativeLog::set_enabled(enabled == JNI_TRUE);
}

// nativeIsLogEnabled() -> boolean
JNIEXPORT jboolean JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeIsLogEnabled(
        JNIEnv* env, jclass clazz){
    return NativeLog::is_enabled() ? JNI_TRUE : JNI_FALSE;
}

// nativeClearLog() -> void
JNIEXPORT void JNICALL Java_cn_sanrol_canalize_world_NativeQueryBridge_nativeClearLog(
        JNIEnv* env, jclass clazz){
    NativeLog::clear();
}

} // extern "C"
