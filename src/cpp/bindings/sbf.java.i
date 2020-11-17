/*
 * Copyright 2014-2018 Neueda Ltd.
 */
%pragma(java) jniclasscode=%{
     // jniclasscode pragma code: Static block so that the JNI class loads the C++ DLL/shared object when the class is loaded
     static {
         try {
             System.loadLibrary("Sbf");
         } catch (UnsatisfiedLinkError e) {
             System.err.println("Native code library failed to load.\n" + e);
             System.exit(1);
         }
     }
%}

SWIG_JAVABODY_PROXY(public, public, SWIGTYPE)
SWIG_JAVABODY_TYPEWRAPPER(public, public, public, SWIGTYPE)

%include "exception.i"

// simple exception handler
%exception {
    try {
        $action
    } catch (std::exception &e) {
        std::string s("sbf-error: "), s2(e.what());
        s = s + s2;
        SWIG_exception(SWIG_RuntimeError, s.c_str());
    }
}

/*
 * Map C++ Buffer to Java byte[]
 */
%{
#include <stdexcept>
#include "jni.h"

static JavaVM *cached_jvm = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    cached_jvm = jvm;
    return JNI_VERSION_1_2;
}

static JNIEnv * JNU_GetEnv() {
    JNIEnv *env;
    jint rc = cached_jvm->GetEnv((void **)&env, JNI_VERSION_1_2);
    if (rc == JNI_EDETACHED)
        throw std::runtime_error("current thread not attached");
    if (rc == JNI_EVERSION)
        throw std::runtime_error("jni version not supported");
    return env;
}
%}

%apply (char *STRING, int LENGTH) { (void* bytes, size_t length) }
%apply (char *STRING, int LENGTH) { (const char* buffer, size_t length) }

SWIG_JAVABODY_PROXY(public, public, SWIGTYPE)
SWIG_JAVABODY_TYPEWRAPPER(public, public, public, SWIGTYPE)

%include "sbf.i"
