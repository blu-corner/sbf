

/*
 * Copyright 2014-2020 Neueda Ltd.
 */
%module(directors="1", thread="1") sbf

%{
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <stdint.h>

#include <SbfLog.hpp>
#include <SbfKeyValue.hpp>
#include <SbfMw.hpp>
#include <SbfQueue.hpp>
#include <SbfPub.hpp>
#include <SbfSub.hpp>
#include <SbfTport.hpp>
#include <SbfBuffer.hpp>
#include <SbfTimer.hpp>
#include <SbfRequestPub.hpp>
#include <SbfRequestSub.hpp>

%}

%include <std_array.i>
%include <stl.i>
%include <stdint.i>

typedef enum
{
    SBF_LOG_DEBUG = 0,
    SBF_LOG_INFO,
    SBF_LOG_WARN,
    SBF_LOG_ERROR,
    SBF_LOG_OFF
} sbfLogLevel;

#define CAP_ALL_MASK 0x00000001
#define CAP_HI_RES_COUNTER 1 << 0
#define SBF_MW_THREAD_LIMIT 64
#define SBF_MW_ALL_THREADS (~0ULL)

// Enable inheritance so java classes can derive from the delegate classes
%feature("director");

/*
 * SbfLog
 */
%extend neueda::SbfLog {
    virtual void log (sbfLogLevel level, const char* msg) {
        self->log (level, "%s", msg);
    }
}
%ignore neueda::SbfLog::log;
%feature("nodirector") neueda::SbfLog::log;
%include "SbfLog.hpp"

/*
 * SbfKeyValue
 */
%ignore neueda::SbfKeyValue::getV;
%feature("nodirector") neueda::SbfKeyValue::getV;
%include "SbfKeyValue.hpp"

/*
 * SbfMw
 */
%include "SbfMw.hpp"

/*
 * SbfQueue
 */
%feature("nodirector") neueda::SbfQueue;
%include "SbfQueue.hpp"

/*
 * SbfPub
 */
%extend neueda::SbfPub {
    virtual void send (const char* buffer, size_t length) {
        void* buff = (void*) malloc(length);
        memcpy (buff, buffer, length + 1);
        self->send (buff, length);
    }
}
// Ignore method we've overridden
%ignore neueda::SbfPub::send;
%feature("nodirector") neueda::SbfPub::send;
%newobject neueda::SbfPub::send;
%include "SbfPub.hpp"

/*
 * SbfSub
 */
%include "SbfSub.hpp"

/*
 * SbfTport
 */
%include "SbfTport.hpp"

/*
 * SbfTimer
 */
%include "SbfTimer.hpp"

/*
 * SbfBuffer
 */
%extend neueda::SbfBuffer {
    SbfBuffer (void* bytes, size_t length) {
        return new neueda::SbfBuffer(bytes, length);
    }
    virtual void setData (const char* buffer, size_t length) {
        void* buff = (void*) malloc (length);
        memcpy (buff, buffer, length+1);
        self->setSize (length);
        self->setData (buff);
    }
}
%newobject neueda::SbfBuffer::setData;
%newobject neueda::SbfBuffer::getByteArray;
%newobject neueda::SbfBuffer::SbfBuffer;
// Ignore methods we don't want to expose or already overridden
%ignore neueda::SbfBuffer::SbfBuffer;
%ignore neueda::SbfBuffer::setData;
%ignore neueda::SbfBuffer::getData;
%ignore neueda::SbfBuffer::setSize;
%include "SbfBuffer.hpp"

/*
 * SbfRequestPub
 */
%extend neueda::SbfRequestPub {
    virtual void send (const char* buffer, size_t length) {
        void* buff = (void*) malloc(length);
        memcpy (buff, buffer, length + 1);
        self->send (buff, length);
    }
}
%newobject neueda::SbfRequestPub::send;
// Ignore method we've overridden
%ignore neueda::SbfRequestPub::send;
%feature("nodirector") neueda::SbfRequestPub::send;
%include "SbfRequestPub.hpp"

/*
 * SbfRequestSub
 */
%extend neueda::SbfRequestSub {
    virtual void reply (struct sbfRequestImpl* req, const char* buffer, size_t length) {
        void* buff = (void*) malloc(length);
        memcpy (buff, buffer, length+1);
        self->reply (req, buff, length);
    }
}
%newobject neueda::SbfRequestSub::reply;
// Ignore method we've overridden
%ignore neueda::SbfRequestSub::reply;
%include "SbfRequestSub.hpp"
