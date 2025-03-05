call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

set QTDIR=c:\Qt\5.15.2\msvc2019_64
set PATH=%QTDIR%\bin;%PATH%

set DEBUG_DIR=..\bld\bin\Debug
set RELEASE_DIR=..\bld\bin\Release
set APP=SwitchDisplay.exe

windeployqt.exe --debug --compiler-runtime %DEBUG_DIR%\%APP%

windeployqt.exe --release --compiler-runtime %RELEASE_DIR%\%APP%
