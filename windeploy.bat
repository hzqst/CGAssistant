for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set VCINSTALLDIR=%%i\VC
)

cd build

windeployqt CGAssistant.exe