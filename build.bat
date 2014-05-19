@echo off

set PATH=%PATH%;C:\Qt\Qt5.2.1\5.2.1\msvc2010\bin

pushd .
C:
cd "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin"
call vcvars32.bat
popd

nmake distclean
qmake -makefile -win32 -o Makefile build.pro
nmake install
