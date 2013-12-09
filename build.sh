#!/bin/sh

(
    make distclean
    qmake-qt4 -makefile -unix -o Makefile build.pro
    make && make install
) 2>&1|tee build.log

egrep 'warning: ' build.log 2>/dev/null | tee -a build.log >&2
