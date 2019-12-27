# CGAssistant
CGAssistant is a helper for you to play CrossGate with highly automated game experience.

Only cg_item_6000 supported for now

QQ Group: 300303705

The latest package can be cloned or downloaded from repo https://github.com/hzqst/CGAssistantJS

### Requirement

Visual Studio 2017

QT 5.12 for msvc2017

Boost 1.69 (using "git submodule update --init --recursive" command)

NodeJS 8.9 windows x86 (download it from https://nodejs.org/dist/v8.9.0/ or other mirrors)

node-gyp (check https://www.npmjs.com/package/node-gyp how to install node-gyp)

## Build

Let's assume that you have all the requirement installed correctly.

1. cd to /CGAssistant

2. run "call boost32md.bat" and "call boost32mt.bat", wait until all static libs are generated.

3. open CGAssistant.sln with Visual Studio 2017

4. compile vcproject "CGAHook"

5. run "call nodegyp.bat" , wait until the node native module is generated.

6. open CGAssistant/CGAssistant.pro with QtCreator, go compile.

7. run "deploy.bat"

8. now you have all binaries copied into /build directory.
