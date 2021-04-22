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
%include <swiginterface.i>

%javaconst(1);
%javaconst(0) CAP_ALL_MASK;
%javaconst(0) CAP_HI_RES_COUNTER;
%javaconst(0) SBF_MW_THREAD_LIMIT;
%javaconst(0) SBF_MW_ALL_THREADS;

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

/*
 * SbfBuffer
 */
%extend neueda::SbfBuffer {
    virtual jbyteArray getByteArray () {
        JNIEnv* jenv = JNU_GetEnv();
        if (jenv != 0)
        {
            jbyteArray jb = (jenv)->NewByteArray (self->getSize());
            (jenv)->SetByteArrayRegion(jb,
                                       0,
                                       self->getSize (),
                                       (jbyte *)self->getData ());
            return jb;
        }
        return NULL;
    }
}
%newobject neueda::SbfBuffer::getByteArray;

%apply (char *STRING, int LENGTH) { (void* bytes, size_t length) }
%apply (char *STRING, int LENGTH) { (const char* buffer, size_t length) }

SWIG_JAVABODY_PROXY(public, public, SWIGTYPE)
SWIG_JAVABODY_TYPEWRAPPER(public, public, public, SWIGTYPE)

%include "sbf.i"
