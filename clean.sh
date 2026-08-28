#!/bin/sh
#
#  Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
#
# ┌─ What : 清理 TQt3 构建树与全部 6 个 KDE 模块的构建产物及统一安装暂存区（staging/）
# │  Why  : build.sh 依赖 build/ 内的 configured/built/installed 标记文件做断点续跑，
# │         并把 make install 经 DESTDIR 重定向到 ./staging——想全量重新构建必须把
# │         标记连同产物、暂存区一起删掉；模块清单必须与 build.sh 的构建清单保持同步
# │         （内容同步更新要求见 AGENTS.md §6 第 7 条）
# │  Who  : 由维护者在仓库根目录手工执行；执行后下次 ./build.sh 将从 cmake 配置开始全量重建
# │  When : 需要干净重建时（更换编译器、修改 CMakeLists、大范围更新源码等）
# │  Where: 仓库根目录；只删各模块 build/ 与根下 staging/，不碰源码、不碰系统目录
# │  How  : 见下方伪代码
#
# 伪代码：
#   1. 删除 tqt3-build/（build.sh 从 tqt3/ pristine 拷出的构建树，含补丁与标记）
#   2. 模块清单 = kdelibs kdebase kdegames kdeutils kdenetwork kdetoys
#      （与 build.sh 主循环的清单逐项一致，任何一侧增删模块必须同步另一侧）
#   3. 逐个删除 <模块>/build
#   4. 删除根下 staging/（build.sh 的 DESTDIR 暂存区）

rm -rf tqt3-build
for i in kdelibs kdebase kdegames kdeutils kdenetwork kdetoys; do
	rm -rf "$i/build"
done
rm -rf staging
