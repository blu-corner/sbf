#!/bin/sh

O="-O2"
while getopts ":d" o; do
    case $o in
        d)
	    O="-O0"
            ;;
        \?)
            echo "usage: $0 [-d]" >&2
	    exit 1
            ;;
    esac
done

for try in qmake-qt4 qmake; do
    export QMAKE=$try
    which $QMAKE >/dev/null 2>&1 && break
done
QMAKE=$(which $QMAKE)

[ -f /proc/cpuinfo ] && J="-j$(grep -c ^processor /proc/cpuinfo)" || J="-j4"
(
    make distclean
    $QMAKE -makefile -o Makefile "O=$O" build.pro
    make $J && make install
) 2>&1|tee build.log

tput setaf 3
egrep 'warning: ' build.log 2>/dev/null | tee -a build.log >&2
tput sgr0
