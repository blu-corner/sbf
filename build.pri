top_src = $$PWD
top_build = $$PWD/install

CONFIG += debug plugin
CONFIG -= qt

OBJECTS_DIR = .obj
DESTDIR = .lib

INCLUDEPATH += \
    $$top_src/core \
    $$top_src/common \
    $$top_src/transport
QMAKE_LIBDIR += \
    $$top_src/core/.lib \
    $$top_src/common/.lib \
    $$top_src/transport/.lib

unix {
    INCLUDEPATH += \
        $$top_src/thirdparty/linux/libevent/include

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += \
            $$top_src/thirdparty/linux/libevent/lib64/libevent.a \
            $$top_src/thirdparty/linux/libevent/lib64/libevent_pthreads.a
    } else {
        LIBS += \
            $$top_src/thirdparty/linux/libevent/lib32/libevent.a \
            $$top_src/thirdparty/linux/libevent/lib32/libevent_pthreads.a
    }

    QMAKE_CFLAGS += \
        -std=gnu99 \
        -O2 \
        -pthread \
        -Wall \
        -Wextra \
        -Wbad-function-cast \
        -Wcast-align \
        -Wfloat-equal \
        -Wformat=2 \
        -Wno-format-y2k \
        -Wno-missing-field-initializers \
        -Wno-unused-parameter \
        -Wpointer-arith \
        -Wsign-compare \
        -Wwrite-strings
    #QMAKE_CFLAGS_DEBUG += \
    #    -O0
    QMAKE_LFLAGS += \
        -pthread \
        -Wl,--no-as-needed
    LIBS += \
        -ldl \
        -lrt
}

windows {
    CONFIG += staticlib
    DEFINES -= UNICODE

    INCLUDEPATH += \
        $$top_src/thirdparty/win32/libevent/include \
        $$top_src/thirdparty/win32/pcre/include

    QMAKE_LIBDIR += \
        $$top_src/thirdparty/win32/libevent/lib \
        $$top_src/thirdparty/win32/pcre/lib
    LIBS += \
        advapi32.lib \
        libevent.lib \
        pcre3.lib \
        pcreposix3.lib \
        ws2_32.lib
    QMAKE_LFLAGS += \
        /NODEFAULTLIB:libcmt.lib

    QMAKE_POST_LINK += \
        if not exist $$shell_path($$top_build/bin) \
	mkdir $$shell_path($$top_build/bin) && \
        copy /Y $$shell_path($$top_src/thirdparty/win32/pcre/lib/*.dll) \
	$$shell_path($$top_build/bin)

    QMAKE_CFLAGS_RELEASE += -MT
    QMAKE_CFLAGS_RELEASE -= -MD
    QMAKE_CXXFLAGS_RELEASE += -MT
    QMAKE_CXXFLAGS_RELEASE -= -MD

    QMAKE_CFLAGS_DEBUG += -MTd
    QMAKE_CFLAGS_DEBUG -= -MDd
    QMAKE_CXXFLAGS_DEBUG += -MTd
    QMAKE_CXXFLAGS_DEBUG -= -MDd
}
