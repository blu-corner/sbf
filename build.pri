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
    QMAKE_LIBS += \
        -ldl \
        -lrt
}

windows {
    INCLUDEPATH += \
        $$top_src/thirdparty/win32/libevent/include

    QMAKE_LIBDIR += \
        $$top_src/thirdparty/win32/libevent/lib
}
