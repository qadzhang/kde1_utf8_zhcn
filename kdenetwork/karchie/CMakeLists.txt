# ┌─ What : karchie(Archie 文件搜索客户端)的 CMake 构建定义——2026-08 翻译自 Makefile.am
# │  Why  : 迁移 CMake 时未跟进被上层注释,现补齐接入(源清单取自 Makefile.am;
# │         其内核为 C 写的 prospero 客户端,与 C++ GUI 混编)
# │  Who  : 上层 kdenetwork/CMakeLists.txt add_subdirectory(karchie)
# │  When : cmake 配置期生成构建规则
# │  Where: kdenetwork/karchie/CMakeLists.txt(样板:kbiff/CMakeLists.txt)
# │  How  : KA*.cpp 为 GUI(含 moc),a*.c/vl*.c/… 为内核;链接 kdecore/kdeui/kfile;
# │         安装 bin/图标/kdelnk/karchierc 配置
# ────────────────────────────────────────────────────────────────────
include_directories(
    ${KDE1_INCLUDE_DIR}
)

add_definitions(-Dlinux -DKARCHIE -DKARCHIE_VERSION="1.1.2")  # Makefile.am 的 AM_CPPFLAGS 等价物

set(karchie_SRCS
    KArchie.cpp
    KAMenu.cpp
    KAQuery.cpp
    KAQueryList.cpp
    KAQueryFile.cpp
    KASettings.cpp
    KASettingsTypes.cpp
    KAStatusBar.cpp
    KAView.cpp
    aquery.c
    dirsend.c
    get_vdir.c
    stcopy.c
    vl_comp.c
    atalloc.c
    get_pauth.c
    ptalloc.c
    support.c
    vlalloc.c
)

# 本目录各 .cpp 均已 #include "*.moc"(KDE1 原惯例),moc 输出并入编译单元
set(karchie_MOC
    KArchie.h
    KAMenu.h
    KAQuery.h
    KAQueryList.h
    KAQueryFile.h
    KASettings.h
    KAStatusBar.h
    KAView.h
)

TQT3_WRAP_CPP(MOC_FILES ${karchie_MOC})

add_executable(karchie ${karchie_SRCS} ${MOC_FILES})
target_link_libraries(karchie
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    ${KDE1_KFM}
)

install(TARGETS karchie RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES karchie.xpm DESTINATION ${KDE1_ICONDIR})
install(FILES mini-karchie.xpm DESTINATION ${KDE1_ICONDIR}/mini)
install(FILES karchie.kdelnk DESTINATION ${KDE1_APPSDIR}/Internet)
install(FILES karchierc DESTINATION ${KDE1_CONFDIR})
