# CGAssistant

![](hzqst/CGAssistant/raw/master/img/logo.png)

CGAssistant is a helper for you to play CrossGate with highly automated game experience.

Only cg_item_6000 supported for now

QQ Group: 300303705

The latest package can be cloned or downloaded from https://github.com/hzqst/CGAssistantJS or https://gitee.com/hzqst/CGAssistantJS

### Requirement

Visual Studio 2017 with C++

QT 5.12 for msvc2017, win32 (x86)

NodeJS 8.9 windows x86 (download it from https://nodejs.org/dist/v8.9.0/ or other mirrors)

node-gyp (check https://www.npmjs.com/package/node-gyp how to install node-gyp)

## Build

Let's assume that you have all requirements installed correctly.

1. Add "Qt5.xxx\Tools\QtCreator\bin\" and "Qt5.xxx\5.xxx\msvc2017\bin\" to your system environment variable "PATH" just like what is mentioned in the snapshot below.

![](hzqst/CGAssistant/raw/master/img/1.png)

2. If your msvc2017 is not installed in default directory, edit every "build-*.bat", change "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" to where your "vcvars32.bat" is located, just like what is mentioned in the snapshot below.

![](hzqst/CGAssistant/raw/master/img/2.png)

3. Run "init-dependencies.bat", wait until all required submodules / dependencies are pulled. (this may takes couple of minutes, depending on your network connection and download speed)

4. Run "build-boost.bat", wait until all boost static libs are generated. (this may takes couple of minutes, depending on your CPU performance)

5. Run "build-CGAHook.bat", wait until "CGAHook.dll" is generated under "build" directory.

5. Run "build-nodegyp.bat", wait until "node_cga.node" is generated under "build/Release" directory.

6. Run "build-qhttp.bat", wait until "qhttp.dll" is generated under "build" directory.

7. Run "build-CGAssiatant.bat", wait until "CGAssistant.exe" is generated under "build" directory.

8. Run "windeploy.bat"

Now you have all compiled binaries copied into "build" directory, if no error occured while compiling.
