#!/bin/sh
#
#  Modified for the KDE1 Revival Project, 2026
#  Maintainer: <维护者姓名> <邮箱>
#  Modifications written with GLM-5.3 (Z.ai)
#  （本脚本继承自上游 NishiOwO/kde1；2026-08 起改为「构建 + DESTDIR 暂存」模式，
#    取消 sudo 直接安装——正式安装一律通过 deb 包，见 AGENTS.md §1 目标 5；
#    2026-08 底座由 Qt 1.45 更换为 TQt3 r14.1.6，构建段见下方 tqt3 部分）
#
# ┌─ What : 一键按序构建 TQt3 底座与 6 个 KDE1 模块，安装结果统一装入 ./staging 暂存区
# │  Why  : KDE 1 各模块有严格的构建顺序依赖（各模块要用 tqt3 的头文件/库/moc，
# │         kdelibs 之后才是应用），须按序构建；同时本项目规定正式安装只经 deb 包、
# │         构建脚本不得 sudo 写入系统目录，故统一重定向到仓库内暂存区，
# │         开发期直接从暂存区运行验证——零提权、零系统侵入
# │  Who  : 由维护者在仓库根目录执行；与 clean.sh 逆向配合（清理 tqt3-build/、
# │         各模块 build/ 与 staging/）
# │  When : 源码就位后执行；任一步失败可重复执行——已成功的步骤凭标记文件跳过
# │  Where: 仓库根目录；TQt3 构建于 tqt3-build/（tqt3/ 快照保持 pristine 永不落笔），
# │         KDE 模块编译产物在各模块 build/ 子目录，安装暂存于 ./staging/<prefix>
# │  How  : 见下方伪代码
#
# 伪代码：
#   1. 平台探测：若是 NetBSD，导出 X11R7/pkg 的头文件与库搜索路径（其他平台跳过）
#   2. 解析 --prefix=（默认 /usr/kde1）：
#        a. prefix 是 deb 安装后的最终系统路径；KDE 模块的 CMake 安装映射到
#           ./staging<prefix>；TQt3 安装到 ./staging<prefix>/tqt3（独立子树，
#           卸载干净、TQTDIR 明确）
#        b. 把 staging<prefix>/bin 与 staging<prefix>/tqt3/bin 加入 PATH ——
#           后续模块的 CMake 经 PATH 找到刚安装的 tqmoc
#        c. 把两个 lib 目录加入 LD_LIBRARY_PATH —— 后续模块链接时能找到 libtqt-mt
#        d. 给 cmake 追加 CMAKE_PREFIX_PATH=staging<prefix> —— 供 find_package/
#           find_path 命中暂存区里先前模块的安装
#   3. 并发度：取 /proc/cpuinfo 中 processor 行数；取不到（容器等）回退 4
#   4. TQt3 构建（tqt3/ 快照零修改，章程红线）：
#        a. 若 tqt3-build/ 不存在：拷贝 tqt3/ → tqt3-build/，按文件名序打入
#           tqt3-patches/*.patch（当前 3 个：qstring 的 UTF-8 比较修复、GIF 解码器启用、
#           见 tqt3-patches/README.md）
#        b. 无 configured 标记则 configure（thread/xft/xrender/system 三方库/
#           -L 多架构库路径；prefix 用最终真实路径使插件路径烧入正确值）
#        c. 无 built 标记则 make -jN
#        d. 无 installed 标记则手动拷贝产物到暂存区（bin/lib/include/plugins/
#           mkspecs；include 用 cp -rL 解符号链接——不用 make install，
#           因 TQt3 的 qmake 会把 prefix 相对化、与 INSTALL_ROOT 拼接出坏路径）
#   5. 对 6 个 KDE 模块依序（模块清单与 clean.sh 必须保持同步，见 AGENTS.md §6 第 7 条）：
#        mkdir build/ → 无 configured 标记则 cmake 配置并落标记 →
#        无 built 标记则 make -jN 并落标记 →
#        无 installed 标记则 make install DESTDIR=<仓库>/staging 并落标记；
#        任一步失败：向 stderr 输出含位置与原因的中文错误并立即退出非零
#      （标记文件即断点续跑机制：成功一步才落标记，重跑只补做缺失的步骤）
#   6. 全部完成后输出从暂存区运行桌面的环境变量提示（含 TQTDIR 与
#      QT_PLUGIN_PATH——后者保证开发期从暂存区加载图片格式插件）

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
# KDE1_REPO_ROOT：FindTQt3.cmake 定位 port/ 脚手架的仓库根（安装版探测文件
# 自身路径推不出仓库根，必须显式传入）
args="$args -DKDE1_REPO_ROOT=$PWD"
# ── KDE 模块找包辅助（What/Why）：kdelibs 把 FindKDE1.cmake 装到 <CMAKE_ROOT>/Modules、
#    且 KDE1InstallDirs.cmake 硬编码 /usr/kde1 真实路径——DESTDIR 暂存模式下二者均不可达，
#    故显式传入模块搜索路径与全部 find 结果的 cache 值（find_path/find_library 见 cache
#    已定义即跳过），使 kdelibs 之后的 5 个模块在零系统安装下互相找到暂存区里的产物
args="$args -DCMAKE_MODULE_PATH=$staging/usr/share/cmake-3.25/Modules"
args="$args -DKDE1_INCLUDE_DIR=$staging$prefix/include/kde1"
for lib in kdecore kdeui kfile kfm kimgio khtmlw jscript mediatool kspell; do
	args="$args -DKDE1_`echo $lib | tr a-z A-Z`=$staging$prefix/lib/lib$lib.so"
done
PATH="$staging$prefix/bin:$staging$prefix/tqt3/bin:$PATH"
LD_LIBRARY_PATH="$staging$prefix/lib:$staging$prefix/tqt3/lib:$LD_LIBRARY_PATH"
export PATH LD_LIBRARY_PATH

# ── 并发度：/proc/cpuinfo 的 processor 行数即逻辑核数；取不到时回退 4
count=`grep processor /proc/cpuinfo | wc -l | sed "s/ //g"`
if [ "$count" = "0" ]; then
	count=4
fi

# ┌─ TQt3 底座构建段（取代原 qt1 段） ─────────────────────────────────────────
# │ What : 在 tqt3-build/ 中构建 TQt3 r14.1.6 并把产物拷入暂存区 <prefix>/tqt3
# │ Why  : tqt3/ 是上游 pristine 快照（零修改红线）；一切必要修改以
# │        tqt3-patches/*.patch 形式存在、构建时打入 tqt3-build/——快照与补丁
# │        分离，tqt3 升级时重拉新快照即可（git 地址与版本记录于 README.md）
# │ When : 首次构建或 tqt3-patches/ 变化后（删 tqt3-build/ 强制重来）
# └─ 三级标记 configured/built/installed 与 KDE 模块一致，断点续跑
tqt3dir="$staging$prefix/tqt3"
if [ ! -d tqt3-build ]; then
	echo "=== TQt3：从 pristine 快照建立构建树 tqt3-build/"
	cp -a tqt3 tqt3-build || { echo "错误：拷贝 tqt3/ → tqt3-build/ 失败（磁盘空间？）" >&2; exit 1; }
	# 按文件名序打入 tqt3-patches/ 下的全部补丁（目录为空则跳过）
	for p in tqt3-patches/*.patch; do
		[ -e "$p" ] || continue
		echo "=== TQt3：打入补丁 $p"
		( cd tqt3-build && patch -p1 --forward < "../$p" ) || { echo "错误：补丁 $p 打入失败，已中止" >&2; exit 1; }
	done
fi
cd tqt3-build || { echo "错误：进入 tqt3-build/ 失败" >&2; exit 1; }
export TQTDIR="$PWD"
if [ ! -e configured ]; then
	# 配置要点：-thread 必需（TQt3 多线程版库）；-xft/-xrender 是现代字体渲染通道；
	# -qt-gif 必需——TQt3 默认关闭 GIF 解码（历史专利遗留），而 KDE1 大量素材是
	# GIF（ksame 石子图/kworldwatch 世界地图/kodo 指南图等），不解码则这些应用
	# 拿到空 pixmap（ksame 牌面全灰、kworldwatch 直接 abort）；GIF 专利早已过期；
	# -system-* 用 Debian 系统库；-L 补多架构库路径（configure 只认传统路径）；
	# -no-nis 规避 SunRPC 头缺失（现代 glibc 已移除）；prefix 用最终真实路径
	# 使 TQT_INSTALL_PLUGINS 等烧入值与 deb 安装后一致
	( echo yes; sleep 2; echo yes ) | ./configure -thread \
		-prefix "$prefix/tqt3" -fast -xft -xrender -qt-gif \
		-no-xcursor -no-xrandr -no-nis -no-imgfmt-mng \
		-system-zlib -system-libpng -system-libjpeg \
		-L/usr/lib/x86_64-linux-gnu -I/usr/include \
		> "$PWD/configure.log" 2>&1 \
	  || { echo "错误：TQt3 configure 失败，详见 tqt3-build/configure.log" >&2; exit 1; }
	touch configured
fi
if [ ! -e built ]; then
	# What/Why：designer/uic 等构建期工具链接主库 libtqt-mt（tquic 处理 .ui 时
	# 动态加载），必须把构建树 lib/ 放进运行期查找路径，否则 tquic 报
	# "cannot open shared object file"（KDE1 虽不用 designer，但顶层 make 会遍历）
	LD_LIBRARY_PATH="$TQTDIR/lib:$LD_LIBRARY_PATH"
	export LD_LIBRARY_PATH
	make -j$count > "$PWD/make.log" 2>&1 \
	  || { echo "错误：TQt3 编译失败，详见 tqt3-build/make.log（并发度 $count）" >&2; exit 1; }
	touch built
fi
if [ ! -e installed ]; then
	# 手动拷贝产物（What/Why）：不用 make install——TQt3 的 qmake 会把 prefix
	# 相对化进子 Makefile，与 INSTALL_ROOT 拼出坏路径；产物清单封闭可控：
	# bin（tqmoc/tqmake 及脚本工具）、lib（主库+prl）、include（cp -rL 解符号
	# 链接成实体，q*.h 与 ntq*.h 双套全备）、plugins（图片格式等运行时插件）、
	# mkspecs（tqmake 运行所需）
	# What/Why：先清空目标再拷（幂等重装）——Qt3 configure 生成的 ntqconfig.h
	# 是只读文件，重复安装时 cp 会拒绝覆盖；顺带保证旧版本残留不累积
	rm -rf "$tqt3dir/bin" "$tqt3dir/lib" "$tqt3dir/include" "$tqt3dir/plugins" "$tqt3dir/mkspecs"
	mkdir -p "$tqt3dir/bin" "$tqt3dir/lib" "$tqt3dir/include" "$tqt3dir/plugins" "$tqt3dir/mkspecs"
	cp bin/tqmoc bin/tqmake bin/tqtfindtr bin/tqtrename140 "$tqt3dir/bin/" 2>/dev/null \
	  || { echo "错误：拷贝 TQt3 bin/ 工具失败（tqmoc/tqmake 应已编译）" >&2; exit 1; }
	cp -P lib/libtqt-mt.so* lib/*.prl "$tqt3dir/lib/" 2>/dev/null
	# What/Why：上游 r14.1.6 删除了 src/attic/ 但 include/ 残留两个悬垂符号链接
	#（qttableview.h/qtmultilineedit.h，上游遗留 bug），cp -rL 解引用时会报错——
	# 拷贝前删除悬垂链接（其目标 attic 已由 port/src 脚手架另行提供）
	find include -maxdepth 1 -type l ! -xtype f -delete 2>/dev/null || true
	cp -rL include/* "$tqt3dir/include/" \
	  || { echo "错误：拷贝 TQt3 include/ 失败" >&2; exit 1; }
	cp -r plugins/imageformats "$tqt3dir/plugins/" 2>/dev/null || true
	cp -r mkspecs/* "$tqt3dir/mkspecs/" 2>/dev/null || true
	touch installed
fi
cd ..

# ── 主构建循环：6 个模块严格按依赖顺序；configured/built/installed 三个标记文件
#    实现断点续跑（已成功的步骤直接跳过）；install 经 DESTDIR 重定向到暂存区，全程无提权；
#    模块清单与 clean.sh 保持同步
for i in kdelibs kdebase kdegames kdegraphics kdeutils kdenetwork kdetoys; do
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
#    QT_PLUGIN_PATH：开发期把图片格式插件指向暂存区（安装到系统后无需设置，
#    插件路径已烧入最终真实前缀）
echo "TQt3 与全部 6 个模块构建并暂存完成（未写入任何系统目录）。开发期运行："
echo "  export TQTDIR=$staging$prefix/tqt3"
echo "  export QT_PLUGIN_PATH=$staging$prefix/tqt3/plugins"
echo "  export PATH=$staging$prefix/bin:\$PATH"
echo "  export LD_LIBRARY_PATH=$staging$prefix/lib:$staging$prefix/tqt3/lib"
echo "  export KDEDIR=$staging$prefix"
echo "正式安装请使用 dist/deb/ 下的 deb 包（dpkg -i / apt install），不要直接使用暂存区部署。"
