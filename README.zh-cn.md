# lc-design-app

([English](README.md)/**中文**)

**克隆并运行以快速查看 LCUI 的运行方式以及 LC Design 组件库的效果。**

## 使用

在克隆和运行这个仓库之前，你需要在你的计算机上安装:

- [Git](https://git-scm.com)
- [Node.js](https://nodejs.org/en/download/) (自带 [npm](http://npmjs.com))
- [LCPkg](https://github.com/lc-soft/lcpkg):
    ```bash
    npm install -g lcpkg
    lcpkg setup
    ```
- [CMake](https://cmake.org/) 或 [XMake](https://xmake.io/)

之后，在你的终端里运行：

```bash
# 克隆这个仓库
git clone https://github.com/lc-ui/lc-design-app

# 进入仓库
cd lc-design-app

# 安装 NodeJS 依赖包
npm install

# 安装适用于 x64 CPU 架构的 C/C++ 依赖库
lcpkg install --arch x64

# 以调试模式运行应用程序
lcpkg run start --mode debug
```

## 包括什么

- 一些资源文件。
- 一些能方便你构建应用程序的脚本。
- [CMake](https://cmake.org/) 和 [XMake](https://xmake.io/) 构建工具的配置文件，你可以选择其中一个你熟悉的来使用。
- 简单的数据存储操作实现，在 `src/lib/store.c` 中可找到。
- 用于描述用户界面样式的 [.scss](https://sass-lang.com/guide) 文件。
- 用于显示应用程序信息的 `about` 组件。
- 实现了简单布局和交互的 `home` 视图。
- 提供 [LC Design](https://github.com/lc-ui/lc-design) 组件库的一些组件效果预览的 `preview` 视图。

## 项目结构

- **`app`**: 包含应用程序运行所需的资源文件。你可以将它当成应用程序的工作目录，在发布应用程序时你只需要打包这个目录内的文件。
- **`build`**: 包含构建工具输出的文件。
- **`scripts`**: 包含构建相关的脚本和配置文件。
- **`src`**: 包含源代码。
  - **`ui`**: 包含用户界面相关的源代码。
    - **`stylesheets`**: 包含样式表的源代码。你可以将 [.scss](https://sass-lang.com/guide) 文件放到这里, 它们会在构建时被编译为 css 文件并输出到  `app/assets/stylesheets` 目录。
    - **`components`**: 包含组件的源代码。
    - **`views`**: 包含视图的源代码。
  - **`lib`**: 包含基础库的源代码。

## 许可

[CC0 1.0 (Public Domain)](LICENSE.md)
