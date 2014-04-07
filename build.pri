top_src = $$PWD
top_build = $$PWD/install

CONFIG += debug plugin
CONFIG -= qt

OBJECTS_DIR = .obj
DESTDIR = .lib

INCLUDEPATH += \
    $$top_src/core \
    $$top_src/common \
    $$top_src/requestreply \
    $$top_src/transport
QMAKE_LIBDIR += \
    $$top_src/core/.lib \
    $$top_src/common/.lib \
    $$top_src/requestreply/.lib \
    $$top_src/transport/.lib

unix {
    INCLUDEPATH += \
        $$top_src/thirdparty/libevent/linux/include
    QMAKE_LFLAGS += \
        -pthread \
        -Wl,--no-as-needed
    LIBS += \
        $$top_src/thirdparty/libevent/linux/lib/libevent.so \
        $$top_src/thirdparty/libevent/linux/lib/libevent_pthreads.so \
        -ldl \
        -lrt

    # It is extremely annoying to get qmake to do this once at the end or to
    # create a pro file that does an install but not a build - so do it after
    # every link.
    QMAKE_POST_LINK += \
        mkdir -p $$top_build/lib && \
        cp -f $$top_src/thirdparty/libevent/linux/lib/*.so $$top_build/lib \
        $$escape_expand(\n\t)

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
}

windows {
    CONFIG += staticlib
    DEFINES -= UNICODE

    INCLUDEPATH += \
        $$top_src/thirdparty/libevent/win32/include \
        $$top_src/thirdparty/pcre/win32/include
    QMAKE_LFLAGS += \
        /NODEFAULTLIB:libcmt.lib
    QMAKE_LIBDIR += \
        $$top_src/thirdparty/libevent/win32/lib \
        $$top_src/thirdparty/pcre/win32/lib
    LIBS += \
	-liphlpapi \
        -ladvapi32 \
        -lws2_32 \
        libevent.lib \
        pcre3.lib \
        pcreposix3.lib

    QMAKE_POST_LINK += \
        if not exist $$shell_path($$top_build/bin) \
	mkdir $$shell_path($$top_build/bin) \
	$$escape_expand(\n\t)
    QMAKE_POST_LINK += \
        copy /Y $$shell_path($$top_src/thirdparty/pcre/win32/lib/*.dll) \
	$$shell_path($$top_build/bin) \
	$$escape_expand(\n\t)
    QMAKE_POST_LINK += \
        del $$shell_path($$top_build/bin/*.pdb) \
	$$escape_expand(\n\t)
    QMAKE_POST_LINK += \
        if not exist $$shell_path($$top_build/lib) \
	mkdir $$shell_path($$top_build/lib) \
	$$escape_expand(\n\t)
    QMAKE_POST_LINK += \
        copy /Y $$shell_path($$top_src/thirdparty/pcre/win32/lib/*.lib) \
	$$shell_path($$top_build/lib) \
	$$escape_expand(\n\t)
    QMAKE_POST_LINK += \
        copy /Y $$shell_path($$top_src/thirdparty/libevent/win32/lib/*.lib) \
	$$shell_path($$top_build/lib) \
	$$escape_expand(\n\t)
    QMAKE_POST_LINK += \
        if not exist $$shell_path($$top_build/include) \
	mkdir $$shell_path($$top_build/include) \
	$$escape_expand(\n\t)
    QMAKE_POST_LINK += \
        copy /Y $$shell_path($$top_src/thirdparty/pcre/win32/include/*.h) \
	$$shell_path($$top_build/include) \
	$$escape_expand(\n\t)

    QMAKE_CFLAGS_RELEASE += -MT
    QMAKE_CFLAGS_RELEASE -= -MD
    QMAKE_CXXFLAGS_RELEASE += -MT
    QMAKE_CXXFLAGS_RELEASE -= -MD

    QMAKE_CFLAGS_DEBUG += -MTd
    QMAKE_CFLAGS_DEBUG -= -MDd
    QMAKE_CXXFLAGS_DEBUG += -MTd
    QMAKE_CXXFLAGS_DEBUG -= -MDd
}
