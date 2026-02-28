这是一份为您准备的 **"NeoFetch Pro" (SysInfo CyberPanel)** 项目产品需求文档 (PRD)。该文档旨在指导基于 **Qt (C++/QML)** 的 Windows GUI 软件开发，完美复刻您提供的设计效果图。

---

# 产品需求文档 (PRD) - "NeoFetch Pro" (SysInfo CyberPanel)

| 项目名称 | NeoFetch Pro (代号: CyberPanel) |
| --- | --- |
| **版本** | 1.0 (MVP) |
| **目标平台** | Windows 11 / Windows 10 |
| **开发框架** | Qt 6.x (C++ 业务逻辑, QML 用户界面) |
| **核心理念** | 将 Linux Terminal 的极客美学带入 Windows 11 GUI，打造极致的系统监控体验。 |

---

## 1. 产品概述

### 1.1 背景与目标

本项目源于对 Linux 端 `neofetch` / `fastfetch` 脚本及其 Konsole 美化方案的喜爱。目标是为 Windows 用户开发一款原生 GUI 软件，提供详细的系统硬件、软件和 UI 信息。它不应是一个普通的、乏味的 Windows 窗口，而必须是一个符合 modern **Glassmorphism (亚克力/云母)** 设计语言、同时保留 Linux 命令行神韵的艺术品。

### 1.2 核心设计风格 (基于效果图)

* **材质：** 全面启用 Windows 11 的 **Mica (云母)** 或 **Acrylic (亚克力)** 效果，提供 80%-85% 的半透明窗口背景。
* **配色：** 深紫色墨水黑 (`#1A1A22`) 为底，文字使用浅灰紫色 (`#CDD6F4`)。
* **强调色：** 硬件区-绿色 (`#A6E3A1`), 软件区-暖黄色 (`#F9E2AF`), UI/UX区-天蓝色 (`#89B4FA`)。
* **布局：** 严格遵循非对称的两栏布局。

---

## 2. 用户界面 (UI) 与 交互 (IX) 需求

**注意：** 所有数据必须从真实的 Windows 系统动态采集，且文字排版必须与效果图一一对应。

### 2.1 窗口结构 (`QQuickWindow` / `ApplicationWindow`)

* **[R2101]** **窗口边角：** 必须使用 Windows 11 标准圆角 (Rounded Corners)。
* **[R2102]** **标题栏 (Title Bar)：** 必须是**沉浸式 (Unified Header)** 标题栏，与内容区融为一体。
* **[R2103]** **窗口控制按钮：** 复刻效果图中的极简线性图标（最小化、全屏、关闭）。关闭按钮悬停时显示红色。
* **[R2104]** **滚动条：** 严禁出现标准 Windows 滚动条。必须设计为极细（<4px）的自定义 QML 滚动条，仅在鼠标悬停时显示。

### 2.2 左侧导航栏 (Sidebar)

* **[R2201]** **顶部 Logo 区域：** 放置复刻后的极简文本型 Debian Swirl 标志，使用暗灰色 (`#6C7086`)。
* **[R2202]** **菜单项：** 提供四个线性图标菜单项（Dashboard, Hardware, Software, Logs）。
* **[R2203]** **菜单交互：** 鼠标悬停时，该项背景轻微变亮；选中时，左侧显示一条 2px 的垂直彩虹渐变指示线。
* **[R2204]** **底部状态栏 (亮点)：** 完全复刻效果图底部的 **Rainbow Prompt**。
* 背景是平滑的彩虹渐变。
* 文本格式为：`[用户ID]: ~/[当前目录]` @ `[时间 (HH:mm)]`。此数据需从 Windows 环境变量中获取。



### 2.3 主内容区 (Main Content)

主内容区由四个细线条卡片 (`Rectangle { border.width: 1 }`) 组成，浮动在半透明背景上。所有核心数据标签（例如：“AMD Ryzen...”, “ext4...”, “Clock 2 days...”）必须加载 **`JetBrainsMono Nerd Font`** 字体。

#### 2.3.1 Hardware 卡片 (绿色强调)

* **[R2311]** **标题：** `[--- Hardware ---]` (绿色)。
* **[R2312]** **数据标签：** 使用预定义图标。
* CPU: 动态获取型号和频率 (e.g., AMD Ryzen... @ 3.60 GHz)。
* GPU: 动态获取型号 (e.g., AMD Radeon Vega... [Integrated])。
* Display: 获取主显示器分辨率和刷新率 (e.g., 1440x900 @ 60 Hz [External])。


* **[R2313]** **磁盘占有率 (亮点)：** 将原图的百分比文字转化为 **绿色线性进度条 (`ProgressBar`)**，分别显示C盘、D盘、E盘的使用情况。进度条下方需保留原图的磁盘容量文字说明 (e.g., `11.04 GiB / 48.91 GiB (23%)`)。

#### 2.3.2 Software 卡片 (黄色强调)

* **[R2321]** **标题：** `[--- Software ---]` (黄色)。
* **[R2322]** **数据标签：**
* OS: 动态获取 Windows 版本详细字符串 (e.g., "Windows 11 Pro 22H2 22621.2506 x86_64")。图标使用 Windows 徽标。
* Kernel: 获取 NT 内核版本 (e.g., "NT 10.0.22621")。
* Packages: (MVP版本可选实现) 尝试从 Windows 包管理器 (scoop/choco) 或 Windows 特有位置（如注册表已安装软件列表）获取数量。
* Shell: 显示当前 Windows 默认 Shell (e.g., PowerShell 7.4.0)。



#### 2.3.3 UI/UX 卡片 (蓝色强调)

* **[R2331]** **标题：** `[--- UI/UX ---]` (蓝色)。
* **[R2332]** **数据标签：** 模拟效果图排版。
* DE: 显示 Windows 原生桌面环境 "Windows Explorer (WinUI 3)"。
* WM: 显示 "DWM (Desktop Window Manager)"。
* Font: 显示当前 GUI 软件使用的主要 Nerd Font。
* App: 显示当前软件名称和版本号 (e.g., "neofetch-pro 1.0.0")。



#### 2.3.4 Uptime 卡片 (右下角)

* **[R2341]** 这是一个特殊的卡片，仅包含最重要的数据。
* **[R2342]** **数据标签：** `Clock [天数] days, [分钟数] mins`。
* **[R2343]** **字体排版 (亮点)：** 必须使用效果图中的**加粗数字时钟字体**。

---

## 3. 功能需求 (C++ Back-end)

### 3.1 数据采集服务 (`SystemDataProvider` 类)

需要编写一个 C++ 后端类，利用 WinAPI、WMI 或其他轻量级方法高效采集数据，并将其发布给 QML 界面。

| 功能模块 | WinAPI / WMI 关键词示例 | 备注 |
| --- | --- | --- |
| **CPU 信息** | `GetSystemInfo`, WMI `Win32_Processor` | 型号、频率、核心数 |
| **GPU 信息** | `EnumDisplayDevices`, WMI `Win32_VideoController` | 型号 |
| **磁盘空间** | `GetDiskFreeSpaceEx` | 需支持多分区、进度条百分比 |
| **系统版本** | `GetVersionEx` (需适配清单), 注册表 `ProductName`/`CurrentVersion` | 精确到 Build 号 |
| **运行时间** | `GetTickCount64` | 转换为 天/分 格式 |
| **字体加载** | `QFontDatabase::addApplicationFont` | 用于 QML 界面加载 Nerd Font |

### 3.2 性能与优化

* **[R3201]** **数据采集频率：** 硬件核心数据（CPU使用率 - MVP可选, GPU, 磁盘）每 5 秒更新一次；其余数据（OS版本、字体）仅在软件启动时加载一次。
* **[R3202]** **WMI 优化：** 尽可能避免过度依赖 WMI 查询，因其可能在部分 Windows 系统上性能不佳。优先使用原生 WinAPI。
* **[R3203]** **异步采集：** 所有长耗时的 WMI 查询或 API 调用必须在单独的子线程中运行，严禁阻塞 UI 线程。

---

## 4. 交付物与实施路线图

### 4.1 交付物

1. **源代码：** 包含完整的 Qt Creator 项目、C++ 后端代码和 QML UI 代码。
2. **资源文件：** 必须包含 `JetBrainsMono Nerd Font` 字体文件及效果图用到的极简图标。
3. **构建脚本：** 用于 Windows 平台的 `CMakeLists.txt`。
4. **演示软件：** 一个可直接在 Windows 11 上运行的 `.exe` 演示程序。

### 4.2 实施路线图 (MVP 1.0)

* **阶段 1: 基础骨架 (QML)** (2-3 天)
* 创建 `ApplicationWindow`，启用全透明背景和亚克力效果。
* 搭建非对称的两栏布局，实现左侧状态栏。
* 实现标题栏的沉浸式设计和窗口控制按钮。


* **阶段 2: 卡片式 UI (QML)** (3-4 天)
* 使用细线条边框 Rectangle 创建四个核心卡片。
* 加载 `JetBrainsMono Nerd Font` 并完成 Hardware, Software 卡片的文字排版复刻。
* 重点实现磁盘占有率进度条和 Uptime 卡片的时钟字体排版。


* **阶段 3: C++ 后端采集** (4-5 天)
* 编写 `SystemDataProvider` 类。
* 实现 WinAPI/WMI 异步采集服务，用于填充 CPU, GPU, Disk, OS, Uptime 数据。
* 将后端数据发布给 QML。


* **阶段 4: 细节与联调** (2-3 天)
* 复刻左侧导航栏菜单项的指示线和状态栏的彩虹渐变。
* 优化滚动条和窗口圆角的显示细节。
* 进行全面的性能测试和适配。