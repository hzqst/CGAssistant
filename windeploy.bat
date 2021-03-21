cd build

windeployqt CGAssistant.exe

for /f "usebackq tokens=*" %%i in (`where node`) do (
  set NodePath=%%i
)

if exist "%NodePath%" (
    copy "%NodePath%" "node.exe" /y
)