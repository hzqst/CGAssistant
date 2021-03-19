for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
    
    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86

    cd qhttp

    mkdir 3rdparty
    
    cd 3rdparty

    git clone https://github.com/nodejs/http-parser.git

    cd ..

    qmake qhttp.pro -spec win32-msvc "CONFIG+=32bit" "CONFIG+=qtquickcompiler"

    jom -f MakeFile qmake_all

    jom

    jom clean

    copy "xbin\qhttp.dll" "..\build\"
)