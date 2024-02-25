cd /d "%~dp0"

for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

cd boost

if exist b2.exe goto bjam

call bootstrap.bat

:bjam

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86

    b2 --toolset=msvc-14.2 --with-date_time --with-thread --with-container --with-system --with-locale --with-serialization --with-regex --stagedir="stage" link=static stage
    b2 --toolset=msvc-14.2 --with-date_time --with-thread --with-container --with-system --with-locale --with-serialization --with-regex --stagedir="stage" runtime-link=static link=static stage

    pause
)