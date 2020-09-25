

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
#include <swigBuffer.h>

%}

%include <arrays_java.i>
%include <std_array.i>
%include <stl.i>
%include <swiginterface.i>
%include <stdint.i>


typedef enum
{
    SBF_LOG_DEBUG = 0,
    SBF_LOG_INFO,
    SBF_LOG_WARN,
    SBF_LOG_ERROR,
    SBF_LOG_OFF
} sbfLogLevel;

%javaconst(1);
%javaconst(0) CAP_ALL_MASK;
%javaconst(0) CAP_HI_RES_COUNTER;
%javaconst(0) SBF_MW_THREAD_LIMIT;
%javaconst(0) SBF_MW_ALL_THREADS;
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

    virtual void logData (sbfLogLevel level, neueda::SwigBuffer buff) {
        void* buffer = (void*) malloc(buff.size);
        memcpy (buffer, buff.addr, buff.size+1);
        self->logData (level, buffer, buff.size);
    }
}
%newobject neueda::SbfLog::logData;
%ignore neueda::SbfLog::log;
%ignore neueda::SbfLog::logData;
%include "SbfLog.hpp"

/*
 * SbfKeyValue
 */
%ignore neueda::SbfKeyValue::getV;
%include "SbfKeyValue.hpp"

/*
 * SbfMw
 */
%include "SbfMw.hpp"

/*
 * SbfQueue
 */
%include "SbfQueue.hpp"

/*
 * SbfPub
 */
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
 * Swig buffer used for passing in objects from higher up languages.
 */
%include "swigBuffer.h"

/*
 * SbfBuffer
 */
%extend neueda::SbfBuffer {
    SbfBuffer (neueda::SwigBuffer buf) {
        return new neueda::SbfBuffer(buf.addr, buf.size);
    }
    virtual void setData (neueda::SwigBuffer buff) {
        void* buffer = (void*) malloc(buff.size);
        memcpy (buffer, buff.addr, buff.size+1);
        self->setSize (buff.size);
        self->setData (buffer);
    }

    virtual neueda::SwigBuffer getData () {
        neueda::SwigBuffer buff = { new unsigned char[self->getSize ()], self->getSize () };
        memcpy (buff.addr, self->getData (), buff.size+1);
        return buff;
    }
}
%newobject neueda::SbfBuffer::setData;
%newobject neueda::SbfBuffer::getData;
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
    virtual void send (neueda::SwigBuffer buff) {
        void* buffer = (void*) malloc(buff.size);
        memcpy (buffer, buff.addr, buff.size+1);
        self->send (buffer, buff.size);
    }
}
%newobject neueda::SbfRequestPub::send;
// Ignore method we've overridden
%ignore neueda::SbfRequestPub::send;
%include "SbfRequestPub.hpp"

/*
 * SbfRequestSub
 */
%extend neueda::SbfRequestSub {
    virtual void reply (struct sbfRequestImpl* req, neueda::SwigBuffer buff) {
        void* buffer = (void*) malloc(buff.size);
        memcpy (buffer, buff.addr, buff.size+1);
        self->reply (req, buffer, buff.size);
    }
}
%newobject neueda::SbfRequestSub::reply;
// Ignore method we've overridden
%ignore neueda::SbfRequestSub::reply;
%include "SbfRequestSub.hpp"