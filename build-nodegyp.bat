cd /d "%~dp0"

for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

cd CGANode

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86

    call npm install nan@latest --save

    call node-gyp configure build --arch=ia32

    mkdir "..\build\Release\"

    copy "build\Release\node_cga.node" "..\build\Release\"

)