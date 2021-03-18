call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"

cd qhttp

mkdir 3rdparty
cd 3rdparty

git clone https://github.com/nodejs/http-parser.git

cd ..

qmake qhttp.pro -spec win32-msvc "CONFIG+=qtquickcompiler"

jom -f MakeFile qmake_all

jom

jom clean

copy "xbin\qhttp.dll" "..\build\"

pause