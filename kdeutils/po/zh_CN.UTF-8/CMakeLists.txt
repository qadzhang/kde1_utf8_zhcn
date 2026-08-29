# ┌─ What : kdeutils 各应用的 zh_CN 翻译安装清单
# │  Why  : 本目录此前无 CMakeLists（Makefile.am 时代产物），翻译从未
# │         随 CMake 构建安装；2026-08 接入并对齐其它模块的 install_gmo 用法
# │  Who  : 上层 kdeutils/po/CMakeLists.txt
# │  When : 构建期 msgfmt 生成 .gmo，make install 落盘 LC_MESSAGES/<app>.mo
# │  Where: kdeutils/po/zh_CN.UTF-8/CMakeLists.txt
# │  How  : install_gmo(语言, po 列表...)（kdelibs/cmake/KDE1Macros.cmake）
# ────────────────────────────────────────────────────────────────────
set(GMOFILES zh_CN.UTF-8
ark.po
kab.po
karm.po
kcalc.po
kedit.po
kfloppy.po
khexdit.po
kjots.po
klipper.po
kljettool.po
klpq.po
knotes.po
kpm.po
ktop.po
kwrite.po
)

install_gmo(${GMOFILES})
