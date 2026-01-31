# Linux平台构建指南

## 依赖安装

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgtk-3-dev
```

### Fedora/RHEL
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    mesa-libGL-devel \
    libX11-devel \
    libXrandr-devel \
    libXinerama-devel \
    libXcursor-devel \
    libXi-devel \
    gtk3-devel
```

### Arch Linux
```bash
sudo pacman -S --needed \
    base-devel \
    cmake \
    mesa \
    libx11 \
    libxrandr \
    libxinerama \
    libxcursor \
    libxi \
    gtk3
```

## 构建步骤

1. **生成项目文件**：
   ```bash
   chmod +x scripts/Linux-GenProjects.sh
   ./scripts/Linux-GenProjects.sh
   ```

2. **编译项目**：
   ```bash
   make config=debug    # 调试版本
   make config=release  # 发布版本
   make config=dist     # 分发版本
   ```

3. **运行示例程序**：
   ```bash
   ./bin/Debug-linux-x86_64/Sandbox/Sandbox
   ```

## 平台特定说明

### 文件对话框
当前Linux平台的文件对话框使用简化实现（返回空字符串）。如需完整功能，可后期实现：
- 使用zenity（GNOME桌面）
- 使用kdialog（KDE桌面）
- 使用GTK3原生对话框

### 输入系统
Linux输入系统使用GLFW API，与Windows版本完全相同。

### 窗口系统
使用GLFW的X11后端，自动支持X11和Wayland（通过GLFW 3.4+）。

## 故障排除

### 常见问题

1. **GLFW初始化失败**：
   - 确保安装了所有X11开发库
   - 检查DISPLAY环境变量是否设置

2. **OpenGL上下文创建失败**：
   - 确保安装了Mesa或NVIDIA/AMD驱动
   - 检查glxinfo是否正常工作

3. **链接错误**：
   - 确保所有依赖库已正确安装
   - 检查premake5.lua中的链接选项

### 调试建议

1. 使用`make VERBOSE=1`查看详细构建过程
2. 检查`bin-int/`目录中的中间文件
3. 使用`ldd`检查二进制文件的依赖关系

## 性能优化

### 编译优化
- 发布版本使用`-O2`优化
- 分发版本使用`-O3`和链接时优化

### 运行时优化
- 启用VSync减少屏幕撕裂
- 使用双缓冲减少闪烁

## 平台差异说明

### 与Windows的主要差异
1. 文件对话框：简化实现
2. 动态库：使用`.so`而非`.dll`
3. 线程：使用pthread而非Windows线程API
4. 路径分隔符：使用`/`而非`\`

### 兼容性保证
- 所有核心渲染功能跨平台一致
- 输入系统行为一致
- 事件系统完全兼容