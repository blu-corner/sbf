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
%typemap(jni) neueda::SwigBuffer "jbyteArray";
%typemap(jtype) neueda::SwigBuffer "byte[]";
%typemap(jstype) neueda::SwigBuffer "byte[]";
%typemap(in) neueda::SwigBuffer {
    $1.addr = (unsigned char *) JCALL2(GetByteArrayElements, jenv, $input, 0);
    $1.size = JCALL1(GetArrayLength, jenv, $input);
}
%typemap(argout) neueda::SwigBuffer {
    JCALL3(ReleaseByteArrayElements, jenv, $input, (jbyte *) $1.addr, 0);
}
%typemap(out) neueda::SwigBuffer {
    $result = JCALL1(NewByteArray, jenv, $1.size);
    JCALL4(SetByteArrayRegion, jenv, $result, 0, $1.size, (jbyte *) $1.addr);
    delete[] $1.addr;
}
%typemap(javain) neueda::SwigBuffer "$javainput";
%typemap(javaout) neueda::SwigBuffer { return $jnicall; }

%include "sbf.i"