/*
 * Copyright 2014-2018 Neueda
 */

// binary interfaces
%include "cstring.i"
%include "pybuffer.i"
%pybuffer_mutable_binary(const char *buffer, size_t length);

%feature("autodoc");

%include "exception.i"
%exception {
    try {
        $action
    } catch (std::exception &e) {
        std::string s("codec-error: "), s2(e.what());
        s = s + s2;
        SWIG_exception(SWIG_RuntimeError, s.c_str());
    }
}

/*
 * SbfBuffer
 */
%extend neueda::SbfBuffer {
    void getByteArray (char** oData, size_t* oLength)
    {
        *oLength = self->getSize ();
        *oData = (char*)malloc (*oLength);
        memcpy (*oData, self->getData (), *oLength);
    }
}

//%apply (char *STRING, int LENGTH) { (void* bytes, size_t length) }
//%apply (char *STRING, int LENGTH) { (const char* buffer, size_t length) }
%cstring_output_allocate_size(char** oData, size_t* oLength, free(*$1));

struct sbfMwThreadImpl;
typedef struct sbfMwThreadImpl* sbfMwThread;

%include "sbf.i"
