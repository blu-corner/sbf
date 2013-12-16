pushd .
cd "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin"
call vcvars32.bat
popd
nmake distclean
qmake -makefile -win32 -o Makefile build.pro
nmake install
