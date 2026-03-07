#include "canalize/api/JNIHelper.h"
#include <stdexcept>

namespace Canalize {

    JavaVM* JNIHelper::s_vm = nullptr;
    static thread_local JNIEnv* g_env = nullptr;

    void JNIHelper::init(JavaVM* vm) {
        s_vm = vm;
    }

    JNIEnv* JNIHelper::getEnv() {
        if (!s_vm) return nullptr;
        
        JNIEnv* env = nullptr;
        jint res = s_vm->GetEnv((void**)&env, JNI_VERSION_1_8);
        
        if (res == JNI_EDETACHED) {
            // Attach current thread
            if (s_vm->AttachCurrentThread((void**)&env, nullptr) != JNI_OK) {
                return nullptr; // Failed to attach
            }
            // In a real implementation, we'd need to register a thread-local destructor or use a thread pool hook to detach
        }
        
        return env;
    }

    void JNIHelper::detach() {
        if (s_vm) {
            s_vm->DetachCurrentThread();
        }
    }

    jclass JNIHelper::findClass(const char* name) {
        JNIEnv* env = getEnv();
        if (!env) return nullptr;
        // Note: In JNI, FindClass uses the system class loader if called from a native thread.
        // It might not find your mod classes unless we cached the class loader or a global ref to a class.
        // For simplicity here, we assume standard JNI context or basic classes.
        // For mod classes, we'd typically need to look them up during JNI_OnLoad and cache them.
        jclass localClass = env->FindClass(name);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            return nullptr;
        }
        return localClass;
    }
    
    // ... Simplified implementation for brevity
    jmethodID JNIHelper::getMethodID(jclass clazz, const char* name, const char* sig) {
        JNIEnv* env = getEnv();
        if (!env || !clazz) return nullptr;
        return env->GetMethodID(clazz, name, sig);
    }

    jmethodID JNIHelper::getStaticMethodID(jclass clazz, const char* name, const char* sig) {
        JNIEnv* env = getEnv();
        if (!env || !clazz) return nullptr;
        return env->GetStaticMethodID(clazz, name, sig);
    }
    
    std::string JNIHelper::jstringToString(jstring jStr) {
        JNIEnv* env = getEnv();
        if (!env || !jStr) return "";
        const char* chars = env->GetStringUTFChars(jStr, nullptr);
        std::string ret(chars);
        env->ReleaseStringUTFChars(jStr, chars);
        return ret;
    }

    jstring JNIHelper::stringToJString(const std::string& str) {
        JNIEnv* env = getEnv();
        if (!env) return nullptr;
        return env->NewStringUTF(str.c_str());
    }

}
