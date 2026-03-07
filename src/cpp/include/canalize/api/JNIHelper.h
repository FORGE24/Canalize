#pragma once
#include "../../CanalizeAPI.h"
#include <jni.h>
#include <string>
#include <mutex>

namespace Canalize {

    // Helper for managing JNI environment across threads and calls
    class CANALIZE_API JNIHelper {
    public:
        static void init(JavaVM* vm);
        static JNIEnv* getEnv();
        static void detach(); // Call on thread exit if attached

        // Helpers to interact with Java classes
        static jclass findClass(const char* name);
        static jmethodID getMethodID(jclass clazz, const char* name, const char* sig);
        static jmethodID getStaticMethodID(jclass clazz, const char* name, const char* sig);
        static std::string jstringToString(jstring jStr);
        static jstring stringToJString(const std::string& str);

    private:
        static JavaVM* s_vm;
        // static thread_local JNIEnv* s_env; // Moved to cpp file scope to avoid dll export issues
    };

}
