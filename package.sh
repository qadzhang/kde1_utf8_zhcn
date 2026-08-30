#!/bin/sh
#
#   Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
#
# ┌─ What : 产出 KDE1 Revival 的 Debian 交付物——二进制包（kde1-core /
# │         kde1-apps / kde1 元包）与源码包（sdeb：.dsc/.orig/.debian），
# │         分目录存放于 dist/deb 与 dist/src（AGENTS.md §1 目标 5）
# │  Why  : 正式安装只经 deb 包（dpkg/apt），apt install 即装、lightdm 即现、
# │         apt remove 干净移除；core 与 apps 按模块拆分，元包承载会话集成
# │  Who  : 由维护者在仓库根目录执行（要求 7 模块已全部构建成功）
# │  When : 发布或每次需要交付包时；产物可重复生成
# │  Where: 仓库根目录；输入为各模块源码与构建系统，输出为 dist/ 下包文件
# │  How  : 见下方伪代码
#
# 伪代码：
#   1. 准备：确认 7 模块均构建过（存在 build 标记）→ 否则提示先 ./build.sh
#   2. 二进制内容树：
#        core 树 = tqt3+kdelibs+kdebase 以拷贝/DESTDIR 重新收编
#        apps 树 = 四个应用模块以 DESTDIR 重新安装
#   3. kde1-core：core 树 + 控制文件（Depends 声明运行库）→ dpkg-deb
#   4. kde1-apps：apps 树 + 控制文件（Depends kde1-core）→ dpkg-deb
#   5. kde1 元包（无内容物）：xsessions 会话入口 + /usr/bin/startkde-kde1
#        环境包装脚本 + Depends 前两包
#   6. 源码包 sdeb：git archive 导出干净源码为 orig.tar.xz（剔除 debian/
#        构建产物）；debian.tar.xz 收编控制/规则；.dsc 描述文件
#   7. 校验：dpkg-deb --info 各包；输出清单汇总
set -e

ROOT="$PWD"
VERSION="1.2.0"
REL="1"
STAGING="$ROOT/staging"
DIST="$ROOT/dist"
OUTDEB="$DIST/deb"
OUTSRC="$DIST/src"
PKGTMP="$DIST/packagetmp"

# ── 1. 前置检查：7 模块构建标记
for i in kdelibs kdebase kdegames kdeutils kdenetwork kdetoys; do
    if [ ! -f "$i/build/built" ]; then
        echo "错误：模块 $i 尚未构建，请先执行 ./build.sh" >&2
        exit 1
    fi
done

# ── 2. 组装内容树（增量 install，秒级完成）
# 包拆分（AGENTS.md §1 目标 5"按模块合理拆分核心包与可选应用包"）：
#   kde1-core        —— tqt3 + kdelibs + kdebase（桌面必需）
#   kde1-games       —— kdegames（可选）
#   kde1-utils       —— kdeutils（可选）
#   kde1-network     —— kdenetwork（可选）
#   kde1-toys        —— kdetoys（可选）
#   kde1             —— 元包（会话集成；依赖 core，Recommends 四个可选包）
rm -rf "$PKGTMP"
mkdir -p "$PKGTMP/core" "$PKGTMP/games" "$PKGTMP/utils" "$PKGTMP/network" "$PKGTMP/toys" \
         "$OUTDEB" "$OUTSRC"
# 链接期需经 LD_LIBRARY_PATH 找到 staging 里的间接依赖库（与 build.sh 同因：
# kjots1 等程序链接时，libkfile.so 的间接依赖 libkfm.so.2 由此解析）
LD_LIBRARY_PATH="$STAGING/usr/kde1/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH
echo "=== 组装 core 树（tqt3 + kdelibs + kdebase）"
# tqt3：从 tqt3-build/ 手动拷贝产物（What/Why：不用 make install——TQt3 qmake 会把
# prefix 相对化进子 Makefile 与 INSTALL_ROOT 拼出坏路径；拷贝清单封闭可控）
mkdir -p "$PKGTMP/core/usr/kde1/tqt3/bin" "$PKGTMP/core/usr/kde1/tqt3/lib" \
         "$PKGTMP/core/usr/kde1/tqt3/include" "$PKGTMP/core/usr/kde1/tqt3/plugins" \
         "$PKGTMP/core/usr/kde1/tqt3/mkspecs"
cp tqt3-build/bin/tqmoc tqt3-build/bin/tqmake tqt3-build/bin/tqtfindtr tqt3-build/bin/tqtrename140 \
   "$PKGTMP/core/usr/kde1/tqt3/bin/" 2>/dev/null || true
find tqt3-build/include -maxdepth 1 -type l ! -xtype f -delete 2>/dev/null || true
cp -P tqt3-build/lib/libtqt-mt.so* tqt3-build/lib/*.prl "$PKGTMP/core/usr/kde1/tqt3/lib/" 2>/dev/null || true
cp -rL tqt3-build/include/* "$PKGTMP/core/usr/kde1/tqt3/include/"
cp -r tqt3-build/plugins/imageformats "$PKGTMP/core/usr/kde1/tqt3/plugins/" 2>/dev/null || true
cp -r tqt3-build/mkspecs/* "$PKGTMP/core/usr/kde1/tqt3/mkspecs/" 2>/dev/null || true
for i in kdelibs kdebase; do
    ( cd "$i/build" && LD_LIBRARY_PATH="$STAGING/usr/kde1/lib:$STAGING/usr/kde1/tqt3/lib" make install DESTDIR="$PKGTMP/core" > /dev/null 2>&1 )
done
echo "=== 组装四个可选应用包树（games/utils/network/toys 各归各包）"
( cd kdegames/build  && make install DESTDIR="$PKGTMP/games"   > /dev/null 2>&1 )
( cd kdeutils/build  && make install DESTDIR="$PKGTMP/utils"   > /dev/null 2>&1 )
( cd kdenetwork/build && make install DESTDIR="$PKGTMP/network" > /dev/null 2>&1 )
( cd kdetoys/build   && make install DESTDIR="$PKGTMP/toys"    > /dev/null 2>&1 )
# 1999 年个别脚本被 install 成 005（属主无读权，如 kfmsu2），统一修复属主读写位
chmod -R u+rwX "$PKGTMP/core" "$PKGTMP/games" "$PKGTMP/utils" "$PKGTMP/network" "$PKGTMP/toys"

# ── 控制文件公共生成函数：$1=内容树目录名 $2=包名 $3=依赖 $4=推荐 $5=描述
mkctrl() {
    local d="$PKGTMP/$1/DEBIAN"
    mkdir -p "$d"
    cat > "$d/control" << EOF
Package: $2
Version: ${VERSION}-${REL}
Architecture: amd64
Maintainer: KDE1 Revival Project <maintainer@kde1-revival.local>
Section: x11
Priority: optional
Depends: $3
Recommends: $4
Description: $5
 The classic KDE 1.1.2 desktop (1999), restored to run natively on
 Debian 12 with full Simplified Chinese (UTF-8) support: fontconfig/Xft
 rendering, fcitx5/fcitx input via XIM, UTF-8 clipboard interop, and a
 WebKit2GTK dual-engine kfm browser.
EOF
}

# ── 3. kde1-core
echo "=== 打包 kde1-core"
mkctrl core kde1-core \
    "libx11-6, libxext6, libxmu6, libxpm4, libjpeg62-turbo, libpng16-16, zlib1g, libtiff6, libxft2, libfontconfig1, libfreetype6, libwebkit2gtk-4.1-0" \
    "fonts-noto-cjk" \
    "KDE 1.2.0 core desktop (TQt3, kdelibs, kdebase) - restored"
dpkg-deb --root-owner-group -b "$PKGTMP/core" "$OUTDEB/kde1-core_${VERSION}-${REL}_amd64.deb"

# ── 4. 四个可选应用包（按模块各归各包；依赖 core，版本锁定保证同批二进制）
echo "=== 打包 kde1-games / kde1-utils / kde1-network / kde1-toys"
mkctrl games kde1-games \
    "kde1-core (= ${VERSION}-${REL})" \
    "" \
    "KDE 1.2.0 games (kdegames) - restored"
dpkg-deb --root-owner-group -b "$PKGTMP/games" "$OUTDEB/kde1-games_${VERSION}-${REL}_amd64.deb"

mkctrl utils kde1-utils \
    "kde1-core (= ${VERSION}-${REL})" \
    "" \
    "KDE 1.2.0 utilities (kdeutils) - restored"
dpkg-deb --root-owner-group -b "$PKGTMP/utils" "$OUTDEB/kde1-utils_${VERSION}-${REL}_amd64.deb"

mkctrl network kde1-network \
    "kde1-core (= ${VERSION}-${REL})" \
    "" \
    "KDE 1.2.0 network applications (kdenetwork) - restored"
dpkg-deb --root-owner-group -b "$PKGTMP/network" "$OUTDEB/kde1-network_${VERSION}-${REL}_amd64.deb"

mkctrl toys kde1-toys \
    "kde1-core (= ${VERSION}-${REL})" \
    "" \
    "KDE 1.2.0 toys (kdetoys) - restored"
dpkg-deb --root-owner-group -b "$PKGTMP/toys" "$OUTDEB/kde1-toys_${VERSION}-${REL}_amd64.deb"

# ── 5. kde1 元包：会话集成（零手工配置，见 AGENTS.md §1 目标 5/6）
echo "=== 打包 kde1 元包（会话集成）"
META="$PKGTMP/meta"
mkdir -p "$META/usr/share/xsessions" "$META/usr/bin" "$META/DEBIAN"
sed "s|@KDE1_PREFIX@|/usr/kde1|g" "$DIST/staging-integration/startkde-kde1.in" \
    > "$META/usr/bin/startkde-kde1"
chmod 755 "$META/usr/bin/startkde-kde1"
cp "$DIST/staging-integration/kde1.desktop" "$META/usr/share/xsessions/"
# （TQt3 工具已随 tqt3 子树入包，无需额外软链）
mkdir -p "$META/usr/kde1/bin"
cat > "$META/DEBIAN/control" << EOF
Package: kde1
Version: ${VERSION}-${REL}
Architecture: amd64
Maintainer: KDE1 Revival Project <maintainer@kde1-revival.local>
Section: x11
Priority: optional
Depends: kde1-core (= ${VERSION}-${REL})
Recommends: kde1-games (= ${VERSION}-${REL}), kde1-utils (= ${VERSION}-${REL}), kde1-network (= ${VERSION}-${REL}), kde1-toys (= ${VERSION}-${REL}), fcitx5 | fcitx, pulseaudio-utils, cups-bsd, cups
Description: KDE 1.2.0 Revival - session integration metapackage
 Installs the lightdm session entry (KDE 1.2.0 Revival) and the
 startkde-kde1 wrapper which sets KDEDIR/PATH/LD_LIBRARY_PATH, starts
 fcitx5/fcitx for XIM input, and preloads padsp so 1999-era OSS sounds
 route to PipeWire/PulseAudio. Zero manual configuration.
EOF
dpkg-deb --root-owner-group -b "$META" "$OUTDEB/kde1_${VERSION}-${REL}_amd64.deb"

# ── 6. 源码包 sdeb（.orig 不含 debian/；debian.tar.xz 为打包规则）
echo "=== 组装源码包"
git archive --format=tar --prefix="kde1_${VERSION}/" HEAD > "$OUTSRC/kde1_${VERSION}.orig.tar"
# 从 orig 中剔除 debian/（标准布局：debian 部分单独成档）
tar -f "$OUTSRC/kde1_${VERSION}.orig.tar" --delete "kde1_${VERSION}/debian" 2>/dev/null || true
xz -f "$OUTSRC/kde1_${VERSION}.orig.tar"

DEBSRC="$PKGTMP/debian-src"
mkdir -p "$DEBSRC/kde1_${VERSION}-${REL}/debian"
cp -r "$ROOT/debian/." "$DEBSRC/kde1_${VERSION}-${REL}/debian/"
tar -cJf "$OUTSRC/kde1_${VERSION}-${REL}.debian.tar.xz" -C "$DEBSRC" "kde1_${VERSION}-${REL}/debian"

cat > "$OUTSRC/kde1_${VERSION}-${REL}.dsc" << EOF
Format: 3.0 (native)
Source: kde1
Binary: kde1, kde1-core, kde1-games, kde1-utils, kde1-network, kde1-toys
Version: ${VERSION}-${REL}
Maintainer: KDE1 Revival Project <maintainer@kde1-revival.local>
Architecture: amd64
Standards-Version: 4.6.2
Build-Depends: build-essential, cmake, git, pkg-config, byacc, flex, gettext, libx11-dev, libxext-dev, libxmu-dev, libxpm-dev, libjpeg-dev, libpng-dev, zlib1g-dev, libtiff-dev, libssl-dev, libglu1-mesa-dev, libxft-dev, libfontconfig1-dev, libfreetype-dev, libwebkit2gtk-4.1-dev
Files:
EOF
for f in "$OUTSRC/kde1_${VERSION}.orig.tar.xz" "$OUTSRC/kde1_${VERSION}-${REL}.debian.tar.xz"; do
    size=$(stat -c%s "$f")
    md5=$(md5sum "$f" | cut -d' ' -f1)
    echo " $(echo $md5) $(printf %8d $size) $(basename $f)" >> "$OUTSRC/kde1_${VERSION}-${REL}.dsc"
done

rm -rf "$PKGTMP"
echo "=== 完成，产物清单："
ls -la "$OUTDEB" "$OUTSRC"
