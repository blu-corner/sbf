set PATH=%PATH%;c:\Program Files (x86)\Windows Kits\8.1\Debuggers\x86
pushd .
cd "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin"
call vcvars32.bat
popd
nmake distclean
qmake -makefile -win32 -o Makefile build.pro
nmake install
