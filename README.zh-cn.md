# lcui-router-app

[![GitHub Actions](https://github.com/lc-ui/lcui-router-app/workflows/C%2FC%2B%2B%20CI/badge.svg)](https://github.com/lc-ui/lcui-router-app/actions)

([English](README.md)/**中文**)

一个仿浏览器界面的应用程序，用于展示 LCUI 的图形界面渲染能力和 LCUI Router 的路由导航功能。

[![运行效果图](./screenshot.gif "运行效果图")](./screenshot.gif)

## 使用

在克隆和运行这个仓库之前，你需要在你的计算机上安装:

- [Git](https://git-scm.com)
- [Node.js](https://nodejs.org/en/download/) (自带 [npm](http://npmjs.com))
- [LCPkg](https://github.com/lc-soft/lcpkg)（仅 Windows 用户需要安装它）:
    ```bash
    npm install -g lcpkg
    lcpkg setup
    ```
- [CMake](https://cmake.org/) 或 [XMake](https://xmake.io/)

之后，在你的终端里运行：

```bash
# 克隆这个仓库
git clone https://github.com/lc-ui/lcui-router-app

# 进入仓库
cd lcui-router-app

# 安装 NodeJS 依赖包
npm install

# 安装适用于 x64 CPU 架构的 C/C++ 依赖库
lcpkg install --arch x64

# 直接运行应用程序
npm start
```

其它有用的命令:

```bash
# 配置编译器使用 debug 模式编译
npm run configure -- --mode debug

# 使用 CMake 构建这个项目
npm run configure -- --tool cmake

# 编译样式文件
npm run build:css

# 编译可执行文件
npm run build:bin
```

## 文档

你可以从[这篇文章](https://zhuanlan.zhihu.com/p/115632949)中了解到此项目是如何被设计和开发出来的。

## 包括什么

- 一些资源文件。
- 一些能方便你构建应用程序的脚本。
- [CMake](https://cmake.org/) 和 [XMake](https://xmake.io/) 构建工具的配置文件，你可以选择其中一个你熟悉的来使用。
- 用于描述用户界面样式的 [.scss](https://sass-lang.com/guide) 文件。
- 浏览器界面的简单实现。
- 欢迎页面、关于页面、文件浏览页面、404 页面、新标签页面。
- [LCUI Router](https://github.com/lc-soft/lcui-router) 的路由导航功能示例。
- [LC Design](https://github.com/lc-ui/lc-design) 组件库的使用示例。

## 项目结构

- **`app`**: 包含应用程序运行所需的资源文件。你可以将它当成应用程序的工作目录，在发布应用程序时你只需要打包这个目录内的文件。
- **`build`**: 包含构建工具输出的文件。
- **`config/router.js`**: LCUI Router 的配置文件，可使用 `lcui compile router` 命令将其编译为 `src/lib/router.c` 文件。
- **`scripts`**: 包含构建相关的脚本和配置文件。
- **`src`**: 包含源代码。
  - **`ui`**: 包含用户界面相关的源代码。
    - **`stylesheets`**: 包含样式表的源代码。你可以将 [.scss](https://sass-lang.com/guide) 文件放到这里, 它们会在构建时被编译为 css 文件并输出到  `app/assets/stylesheets` 目录。
    - **`components`**: 包含组件的源代码。
    - **`views`**: 包含视图的源代码。
  - **`lib`**: 包含基础库的源代码。

## 许可

[CC0 1.0 (Public Domain)](LICENSE.md)
