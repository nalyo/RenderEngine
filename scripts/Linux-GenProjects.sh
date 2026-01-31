#!/bin/bash
# Linux构建脚本

echo "生成Linux项目文件..."
./vendor/bin/premake/premake5 gmake2

echo "项目文件已生成，可以使用以下命令构建："
echo "  make config=debug   # 调试版本"
echo "  make config=release # 发布版本"
echo "  make config=dist    # 分发版本"