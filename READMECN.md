# CGAssistant

![](img/logo.png)

[ENGLISH README](README.md)

CGAssistant 是一个免费开源的用于游戏“CrossGate”的自动化程序。

QQ群: 300303705

## 特别声明

本项目仅供学习交流，禁止用于商业用途。

本项目内的所有资源文件和程序，禁止在 GitHub 以外的任何地方进行转载或发布。

本项目的[完整发布包](https://github.com/hzqst/CGAssistant/releases)中附带的所有CGA相关二进制程序（.dll .exe）均从[源代码](https://github.com/hzqst/CGAssistant) 编译而来，源代码接受所有人的监督，并且由[GitHub Actions](https://docs.github.com/en/actions/learn-github-actions)提供的构建服务自动构建。

使用者必须自己对使用后果负责，包括但不限于由项目中的任何程序问题导致的任何游戏账号及虚拟资产损失或损害。

本项目遵循 MIT 协议，如果本特别声明与 MIT 协议有冲突之处，以本特别声明为准。

您使用或者复制了该项目中的任何代码或程序，则视为已接受此声明，请仔细阅读。

您在本声明未发出之时点使用或者复制了本仓库且本人制作的任何代码或项目且此时还在使用，则视为已接受此声明，请仔细阅读。

## 链接

[源码](https://github.com/hzqst/CGAssistant)

[脚本](https://github.com/hzqst/CGAssistantJS)

[下载](https://github.com/hzqst/CGAssistant/releases)

[使用攻略](https://github.com/hzqst/CGAssistant/wiki)

## 编译前置需求

1. [Visual Studio 2017, 和 vc141 工具集](https://visualstudio.microsoft.com/)

2. [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)

3. [QT 5.12 for msvc2017, 32位版本 (x86), 以及 QtCreator](https://www.qt.io/download)

4. [NodeJS v14.16 (32位)](https://nodejs.org/download/release/v14.16.0/)

5. [node-gyp] (https://www.npmjs.com/package/node-gyp)

* QT 5.15 + Visual Studio 2019 也是可以的，但你需要自己调整`build-*.bat`中的配置，让`vswhere`定位到vs2019而非vs2017。

## 编译

假设你已经成功安装了所有的前置需求

1. 把`Qt5.**\Tools\QtCreator\bin\`, `Qt5.*\5.**\msvc20**\bin\` and `Qt5.**\Tools\QtCreator\bin\jom` (如果该目录存在的话) 添加到你的系统环境变量 "PATH" 中，就如下图所示的那样。如果不知道如何添加环境变量请自行上网搜索解决方法。

![](img/1.png)

2. 运行 "init-dep.bat", 等待所有子模块和依赖项目下载完成。 (这一步可能需要花费几分钟时间, 具体取决于你的网速，如果速度很慢或者下载出错请自行尝试科学上网)

3. 运行 "build-boost.bat", 等待boost的所有静态库编译完成。 (这一步可能需要花费数十分钟时间，具体取决于你的CPU性能)

4. 运行 "build-CGAHook.bat", 等待 "CGAHook.dll" 生成成功后会自动复制到 "build" 目录下。

5. 运行 "build-qhttp.bat", 等待 "qhttp.dll" 生成成功后会自动复制到 "build" 目录下。

6. 运行 "build-CGAssiatant.bat", 等待 "CGAssistant.exe" 生成成功后会自动复制到 "build" 目录下。

7. 运行 "build-nodegyp.bat", 等待 "node_cga.node" 生成成功后会自动复制到 "build/Release" 目录下。

8. 运行 "windeploy.bat"

如果没有错误发生，那么所有编译好的二进制文件应该都会在 "build" 目录下。
