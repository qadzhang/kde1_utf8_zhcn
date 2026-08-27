#!/bin/sh
#
#  Modified for the KDE1 Revival Project, 2026
#  Maintainer: <维护者姓名> <邮箱>
#  Modifications written with GLM-5.3 (Z.ai)
#  （本脚本继承自上游 NishiOwO/kde1；2026-08 起改为「构建 + DESTDIR 暂存」模式，
#    取消 sudo 直接安装——正式安装一律通过 deb 包，见 AGENTS.md §1 目标 5）
#
# ┌─ What : 一键按序构建全部 7 个模块，并把 make install 结果统一装入 ./staging 暂存区
# │  Why  : KDE 1 各模块有严格的构建顺序依赖（kdelibs 要用 qt1 的头文件/库，应用要用
# │         kdelibs/kdebase），须按序构建；同时本项目规定正式安装只经 deb 包、构建脚本
# │         不得 sudo 写入系统目录，故用 DESTDIR 把各模块的安装重定向到仓库内暂存区，
# │         开发期直接从暂存区运行验证——零提权、零系统侵入
# │  Who  : 由维护者在仓库根目录执行；与 clean.sh 逆向配合（清理 build/ 与 staging/）
# │  When : 源码就位后执行；任一模块失败后可重复执行——已成功的步骤凭标记文件跳过
# │  Where: 仓库根目录；编译产物在各模块 build/ 子目录，安装暂存于 ./staging/<prefix>
# │  How  : 见下方伪代码
#
# 伪代码：
#   1. 平台探测：若是 NetBSD，导出 X11R7/pkg 的头文件与库搜索路径（其他平台跳过）
#   2. 解析 --prefix=（默认 /usr/kde1）：
#        a. prefix 是 deb 安装后的最终系统路径；本脚本把它映射到 ./staging<prefix>
#        b. 把 staging<prefix>/bin 加入 PATH —— 后续模块的 CMake 经 PATH 找到刚安装的
#           moc-qt1（取代旧版往 /usr/bin 做软链的做法，无需任何提权）
#        c. 把 staging<prefix>/lib 加入 LD_LIBRARY_PATH —— 后续模块链接时能找到刚装的 qt1
#        d. 给 cmake 追加 CMAKE_PREFIX_PATH=staging<prefix> —— 供 find_package/find_path
#           命中暂存区里先前模块的安装（取代旧版依赖真实系统安装）
#   3. 并发度：取 /proc/cpuinfo 中 processor 行数；取不到（容器等）回退 4
#   4. 对 7 个模块依序（模块清单与 clean.sh 必须保持同步，见 AGENTS.md §6 第 7 条）：
#        mkdir build/ → 无 configured 标记则 cmake 配置并落标记 →
#        无 built 标记则 make -jN 并落标记 →
#        无 installed 标记则 make install DESTDIR=<仓库>/staging 并落标记；
#        任一步失败：向 stderr 输出含位置与原因的中文错误并立即退出非零
#      （标记文件即断点续跑机制：成功一步才落标记，重跑只补做缺失的步骤）
#   5. 全部完成后输出从暂存区运行桌面的环境变量提示

# ── 平台适配：NetBSD 的 X11 与第三方包装在 /usr/X11R7 与 /usr/pkg 下，
#    默认搜索路径不含它们，导出后供下方 cmake 调用的编译器拾取；其他平台此段为空操作
args="-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++  -DCMAKE_POLICY_VERSION_MINIMUM=3.5"
if [ "`uname -s`" = "NetBSD" ]; then
	CPPFLAGS="-I/usr/X11R7/include -I/usr/pkg/include"
	LDFLAGS="-L/usr/X11R7/lib -L/usr/pkg/lib"
	CXXFLAGS="$CPPFLAGS"
	CFLAGS="$CPPFLAGS"
	export CXXFLAGS CFLAGS LDFLAGS
fi

# ── 参数解析：目前仅识别 --prefix=（默认 /usr/kde1）；随后组装暂存区路径与
#    PATH/LD_LIBRARY_PATH/CMAKE_PREFIX_PATH 三个查找链（见文件头伪代码第 2 步）
prefix="/usr/kde1"
for i in $@; do
	case "$i" in
		--prefix=*)
			prefix=`echo $i | sed "s/--prefix=//g"`
			;;
	esac
done
staging="$PWD/staging"
args="$args -DCMAKE_INSTALL_PREFIX=$prefix -DCMAKE_PREFIX_PATH=$staging$prefix"
# ── KDE 模块找包辅助（What/Why）：kdelibs 把 FindKDE1.cmake 装到 <CMAKE_ROOT>/Modules、
#    且 KDE1InstallDirs.cmake 硬编码 /usr/kde1 真实路径——DESTDIR 暂存模式下二者均不可达，
#    故显式传入模块搜索路径与全部 find 结果的 cache 值（find_path/find_library 见 cache
#    已定义即跳过），使 qt1 之后的 6 个模块在零系统安装下互相找到暂存区里的产物
args="$args -DCMAKE_MODULE_PATH=$staging/usr/share/cmake-3.25/Modules"
args="$args -DKDE1_INCLUDE_DIR=$staging$prefix/include/kde1"
for lib in kdecore kdeui kfile kfm kimgio khtmlw jscript mediatool kspell; do
	args="$args -DKDE1_`echo $lib | tr a-z A-Z`=$staging$prefix/lib/lib$lib.so"
done
PATH="$staging$prefix/bin:$PATH"
LD_LIBRARY_PATH="$staging$prefix/lib:$LD_LIBRARY_PATH"
export PATH LD_LIBRARY_PATH

# ── 并发度：/proc/cpuinfo 的 processor 行数即逻辑核数；取不到时回退 4
count=`grep processor /proc/cpuinfo | wc -l | sed "s/ //g"`
if [ "$count" = "0" ]; then
	count=4
fi

# ── 主构建循环：7 个模块严格按依赖顺序；configured/built/installed 三个标记文件
#    实现断点续跑（已成功的步骤直接跳过）；install 经 DESTDIR 重定向到暂存区，全程无提权；
#    模块清单与 clean.sh 保持同步
for i in qt1 kdelibs kdebase kdegames kdeutils kdenetwork kdetoys; do
	echo "=== 开始处理模块：$i"
	mkdir -p $i/build
	cd $i/build
	if [ ! -e "configured" ]; then
		cmake .. $args || { echo "错误：模块 $i 的 cmake 配置失败，已中止（目录：仓库根/$i/build）" >&2; exit 1; }
		touch configured
	fi
	if [ ! -e "built" ]; then
		make -j$count || { echo "错误：模块 $i 的编译失败，已中止（目录：仓库根/$i/build，并发度 $count）" >&2; exit 1; }
		touch built
	fi
	if [ ! -e "installed" ]; then
		make install DESTDIR="$staging" || { echo "错误：模块 $i 安装到暂存区失败，已中止（DESTDIR：$staging）" >&2; exit 1; }
		touch installed
	fi
	cd ../..
done

# ── 收尾提示：开发期从暂存区运行的方法；正式安装只走 deb 包
echo "全部 7 个模块构建并暂存完成（未写入任何系统目录）。开发期运行："
echo "  export PATH=$staging$prefix/bin:\$PATH"
echo "  export LD_LIBRARY_PATH=$staging$prefix/lib"
echo "  export KDEDIR=$staging$prefix"
echo "正式安装请使用 dist/deb/ 下的 deb 包（dpkg -i / apt install），不要直接使用暂存区部署。"
