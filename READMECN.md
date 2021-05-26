# CGAssistant

![](img/logo.png)

[英文README](README.md)

CGAssistant 是一个用于游戏CrossGate（中译：魔力宝贝）的工具，可以让你拥有自动化的游戏体验。

QQ群: 300303705

## 特别声明

本项目仅供学习交流，禁止用于商业用途。

本项目内的所有资源文件和程序，禁止在 GitHub 以外的任何地方进行转载或发布。

本项目的[完整发布包](https://github.com/hzqst/CGAssistantJS)中附带的所有二进制程序（.dll .exe）均可从[源代码](https://github.com/hzqst/CGAssistant)编译而来，源代码接受所有人的监督，并且由[GitHub Actions](https://docs.github.com/en/actions/learn-github-actions)提供的构建服务自动编译，不存在携带任何恶意程序的可能。

即便如此，本项目对使用者的游戏账号安全仍然不作任何保证，使用者必须自己对使用后果负责，包括但不限于由项目中的任何脚本或程序问题导致的任何游戏账号损失或损害。

本项目遵循 MIT 协议，如果本特别声明与 MIT 协议有冲突之处，以本特别声明为准。

您使用或者复制了该项目中的任何代码或程序，则视为已接受此声明，请仔细阅读。

您在本声明未发出之时点使用或者复制了本仓库且本人制作的任何代码或项目且此时还在使用，则视为已接受此声明，请仔细阅读。

## 链接

[源代码](https://github.com/hzqst/CGAssistant)

[完整发布包 (Github)](https://github.com/hzqst/CGAssistantJS) [完整发布包 (Gitee, 国内镜像)](https://gitee.com/hzqst/CGAssistantJS)

[使用攻略 (Github)](https://github.com/hzqst/CGAssistantJS/wiki) [使用攻略 (Gitee, 国内镜像)](https://gitee.com/hzqst/CGAssistantJS/wikis)

## 编译前置需求

Visual Studio 2017 或 2019

Windows SDK 10 (msvc工程文件里使用的是 Windows SDK version 10.0.17763.0，如果你安装了不同版本的 WindowsSDK 则需要“重定向项目”)

VC141 或者 VC142 工具集 (msvc工程文件里使用的是 VC141工具集，如果你需要使用VC142工具集请自行修改vcxproject和build-CGAHook.bat中的PlatformToolset字段)

QT 5.12+ for msvc2017, win32 (x86)

NodeJS v14+ (32位 或 64位 均可)

node-gyp (建议跟随 https://www.npmjs.com/package/node-gyp 的指示来安装 node-gyp)

## 编译

假设你已经成功安装了所有的前置需求

1. 把 "Qt5.xxx\Tools\QtCreator\bin\" 和 "Qt5.xxx\5.xxx\msvc2017\bin\" 添加到你的系统环境变量 "PATH" 中，就如下图所示的那样。如果不知道如何添加环境变量请自行百度。

![](img/1.png)

2. 运行 "init-dep.bat", 等待所有子模块和依赖项目下载完成。 (这一步可能需要花费几分钟时间, 具体取决于你的网速，如果速度很慢或者下载出错建议尝试使用proxy)

3. 运行 "build-boost.bat", 等待boost的所有静态库编译完成。 (这一步可能需要花费十几分钟时间，具体取决于你的CPU性能)

4. 运行 "build-CGAHook.bat", 等待 "CGAHook.dll" 生成成功后会自动复制到 "build" 目录下。

5. 运行 "build-qhttp.bat", 等待 "qhttp.dll" 生成成功后会自动复制到 "build" 目录下。

6. 运行 "build-CGAssiatant.bat", 等待 "CGAssistant.exe" 生成成功后会自动复制到 "build" 目录下。

7. 运行 "build-nodegyp.bat", 等待 "node_cga.node" 生成成功后会自动复制到 "build/Release" 目录下。

8. 运行 "windeploy.bat"

如果没有错误发生，那么所有编译好的二进制文件应该都会在 "build" 目录下。
