call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

set QTDIR=c:\Qt\5.15.2\msvc2019_64
set PATH=%QTDIR%\bin;%PATH%

set DEBUG_DIR=.\bld\Debug
set RELEASE_DIR=.\bld\Release

windeployqt.exe --debug --compiler-runtime %DEBUG_DIR%\SwitchDisplay.exe

windeployqt.exe --release --compiler-runtime %RELEASE_DIR%\SwitchDisplay.exe
