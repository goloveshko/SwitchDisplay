call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

set QTDIR=c:\Qt\6.8.0\msvc2022_64
set PATH=%QTDIR%\bin;%PATH%

cmake -Bbld_qt6 -G "Visual Studio 17 2022" ./src

pause
