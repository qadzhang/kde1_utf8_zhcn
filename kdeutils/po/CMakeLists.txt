# ┌─ What : kdeutils 的 po 子树接入（此前被上层注释、翻译从未安装）
# │  Why  : kdeutils 十余个应用的 zh_CN 翻译长期未随构建进入 staging/deb，
# │         与其它模块（kdegames/kdenetwork/kdetoys 均已接入）不一致
# │  Who  : 上层 kdeutils/CMakeLists.txt add_subdirectory(po)
# │  When : cmake 配置期；install_gmo 宏在构建期 msgfmt、安装期落盘
# │  Where: kdeutils/po/CMakeLists.txt
# │  How  : 只挂 zh_CN.UTF-8（其余语言目录为 Makefile.am 时代产物，无
# │         CMakeLists，不接入）；pot 文件与其它模块一致安装到 KDE1_LOCALE
# ────────────────────────────────────────────────────────────────────
add_subdirectory(zh_CN.UTF-8)

install(FILES
    ark.pot
    kab.pot
    karm.pot
    kcalc.pot
    kedit.pot
    kfloppy.pot
    khexdit.pot
    kjots.pot
    klipper.pot
    kljettool.pot
    klpq.pot
    knotes.pot
    kpm.pot
    ktop.pot
    kwrite.pot
DESTINATION ${KDE1_LOCALE})
