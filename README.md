# SysInfoFetch (NeoFetch Pro)

一个现代化的 Windows 系统信息展示工具，提供清晰、美观的系统硬件和软件信息概览。

## 功能特性

### 仪表板 (Dashboard)
- 显示当前用户名和工作目录
- 操作系统版本信息
- CPU 使用率实时监控
- 内存使用率实时监控

### 硬件信息 (Hardware)
- **CPU 信息**: 处理器型号、核心数、线程数
- **GPU 信息**: 显卡型号和驱动版本
- **显示器信息**: 显示设备详情
- **内存信息**: 总容量、已使用、使用百分比
- **磁盘信息**: 
  - 所有驱动器的文件系统类型
  - 每个分区的容量和已用空间
  - 磁盘硬件型号（通过 WMI 查询）
- **网络信息**: 网络适配器和 IP 地址

### 软件信息 (Software)
- 操作系统详细信息
- 内核版本
- Shell 环境（如 PowerShell、bash 等）

### 日志面板 (Logs)
- 完整的系统信息文本输出
- 实时更新的数据日志

### 界面特性
- 现代化深色主题设计
- 无边框窗口，支持自定义标题栏
- 自动根据内容调整高度，无需滚动条
- 清晰的信息层级和配色方案
- 侧边栏导航，快速切换功能模块
- 自定义应用程序图标（来自 @logo.jpg）

## 技术栈

- **框架**: Qt 5.x (C++)
- **平台**: Windows (Win32 API + WMI)
- **构建系统**: CMake
- **编译器**: MinGW-w64 (GCC)

## 构建要求

### 前置依赖

1. **Qt5 开发库**
   - Qt5Core, Qt5Widgets, Qt5Gui
   - 建议使用 MSYS2 安装:
     ```bash
     pacman -S mingw-w64-x86_64-qt5
     ```

2. **CMake** (3.10+)
   ```bash
   pacman -S cmake
   ```

3. **MinGW-w64 工具链**
   ```bash
   pacman -S mingw-w64-x86_64-toolchain
   ```

### 构建步骤

1. **克隆仓库**
   ```bash
   git clone https://github.com/yourusername/SysInfoFetch.git
   cd SysInfoFetch
   ```

2. **创建构建目录**
   ```bash
   mkdir build
   cd build
   ```

3. **配置项目**
   ```bash
   cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
   ```

4. **编译**
   ```bash
   cmake --build . --config Release
   ```

5. **运行**
    ```bash
    ./NeoFetchPro.exe
    ```

## 下载

预编译的可执行文件可从 [GitHub Releases](https://github.com/alloyapple/SysInfoFetch/releases/tag/v1.0.0) 获取：

### 下载选项

1. **完整包 (推荐)** - `NeoFetchPro-v1.0.0.zip` (约 9.6 MB)
   - 包含所有必需的 DLL 依赖
   - 下载后解压即可直接运行
   - 无需额外安装任何库

2. **单独可执行文件** - `NeoFetchPro.exe` (约 480 KB)
   - 需要手动复制以下 DLL 文件到同一目录:
     - Qt5Core.dll
     - Qt5Gui.dll
     - Qt5Widgets.dll
     - libgcc_s_seh-1.dll
     - libstdc++-6.dll
     - libwinpthread-1.dll
     - platforms/qwindows.dll
   - 可从 [GitHub Releases](https://github.com/alloyapple/SysInfoFetch/releases/tag/v1.0.0) 下载完整包获取这些文件

### 快速开始

```bash
# 下载完整包
# 1. 从 Releases 页面下载 NeoFetchPro-v1.0.0.zip
# 2. 解压到任意目录
# 3. 运行 NeoFetchPro.exe
```

### 故障排除

**问题**: 编译时找不到 Qt 库
**解决**: 确保 Qt5 的 bin 目录在 PATH 环境变量中，或使用 Qt 的 qmake 来生成 Makefile。

**问题**: 链接错误，缺少 WMI 库
**解决**: 确保 CMakeLists.txt 中包含以下库:
- wbemuuid.lib
- ole32.lib
- oleaut32.lib
- uuid.lib

**问题**: 运行时崩溃
**解决**: 确保所有 DLL 都在同一目录或系统 PATH 中，特别是:
- Qt5Core.dll
- Qt5Widgets.dll
- Qt5Gui.dll

## 项目结构

```
SysInfoFetch/
├── CMakeLists.txt          # 构建配置
├── README.md               # 项目说明
├── build/                  # 构建目录（不加入版本控制）
├── src/
│   ├── main.cpp           # 程序入口
│   ├── mainwindow.cpp     # 主窗口实现
│   ├── mainwindow.h       # 主窗口头文件
│   ├── SystemDataProvider.cpp  # 系统数据提供者
│   └── SystemDataProvider.h    # 数据提供者头文件
└── resources/             # 资源文件（如图标等）
```

## 开发说明

### 添加新的系统信息

1. 在 `SystemDataProvider.h` 中添加新的成员函数声明
2. 在 `SystemDataProvider.cpp` 中实现数据获取逻辑
3. 在 `MainWindow::updateData()` 中调用并显示到 UI

### 修改 UI 布局

- 主界面使用 `QStackedWidget` 管理多个面板
- 每个面板使用 `QFrame` + `QVBoxLayout` 构建卡片式设计
- 所有面板遵循统一的样式和配色方案

## 许可证

MIT License

## 贡献

欢迎提交 Issue 和 Pull Request！
