call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"

cd CGAssistant

qmake CGAssistant.pro -spec win32-msvc "CONFIG+=qtquickcompiler"

jom -f MakeFile qmake_all

jom

jom clean