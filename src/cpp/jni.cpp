#include <jni.h>
#include <iostream>
#include "include/WorldLoader.h"

// JNI EXPORTS

void perform_hijack()   { std::cout<<"[Native] Hijack Successful!"<<std::endl; }
void load_model_block() { std::cout<<"[Native] MODEL Block Loaded."<<std::endl; }

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){ 
    WorldLoader::init();
    return JNI_VERSION_1_8; 
}
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved){}

JNIEXPORT void JNICALL Java_cn_sanrol_canalize_Canalize_initNative(
        JNIEnv* env, jclass clazz){ perform_hijack(); load_model_block(); }

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

JNIEXPORT jint JNICALL Java_cn_sanrol_canalize_world_NativeBiomeSource_getBiomeNative(
        JNIEnv* env, jobject thiz, jint x, jint z){
    return WorldLoader::get_biome(x,z);
}

} // extern "C"
