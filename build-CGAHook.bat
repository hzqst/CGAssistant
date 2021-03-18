call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"

MSBuild.exe CGAssistant.sln /t:CGAHook /p:Configuration=Release /p:Platform="x86" /p:PlatformToolset=v141