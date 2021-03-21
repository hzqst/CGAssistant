for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set VCINSTALLDIR=%%i\VC
)

cd build

windeployqt CGAssistant.exe

for /f "usebackq tokens=*" %%i in (`where node`) do (
  set NodePath=%%i
)

if exist "%NodePath%" (
    copy "%NodePath%" "node.exe" /y
)