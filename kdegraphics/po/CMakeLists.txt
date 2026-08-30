# ┌─ What : kdegraphics 的 po 子树接入
# │  Why  : 2026-08 新纳入构建；只挂 zh_CN.UTF-8（GB2312 旧目录已转码，
# │         其余语言目录为 Makefile.am 时代产物不接入）
# │  How  : install_gmo 宏（kdelibs/cmake/KDE1Macros.cmake）
add_subdirectory(zh_CN.UTF-8)

install(FILES
    kdvi.pot
    kfax.pot
    kfract.pot
    kghostview.pot
    kiconedit.pot
    kpaint.pot
    ksnapshot.pot
    kview.pot
    DESTINATION ${KDE1_LOCALE}/zh_CN.UTF-8)
