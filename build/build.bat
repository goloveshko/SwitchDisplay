call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

set QTDIR=c:\Qt\5.15.2\msvc2019_64\
set PATH=%QTDIR%\bin;%PATH%

cmake -B../bld -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release ../src

pause
