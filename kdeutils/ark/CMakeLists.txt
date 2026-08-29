# ┌─ What : ark(归档管理器)的 CMake 构建定义——2026-08 从 Makefile.am 翻译而来
# │  Why  : kdeutils 迁移 CMake 时本应用未跟进而被上层注释,现补齐接入,
# │         使 ark 随构建产出并进入 staging/deb 交付(源清单取自 Makefile.am)
# │  Who  : 上层 kdeutils/CMakeLists.txt add_subdirectory(ark)
# │  When : cmake 配置期生成构建规则
# │  Where: kdeutils/ark/CMakeLists.txt(样板:kjots/CMakeLists.txt)
# │  How  : SRCS 照 Makefile.am 的 ark_SOURCES;MOC 为 METASOURCES 对应四头;
# │         安装 bin/图标/kdelnk/HTML 文档(doc/ 子树 HTML 文件逐项列出)
# ────────────────────────────────────────────────────────────────────
include_directories(
    ${KDE1_INCLUDE_DIR}
)

add_definitions(-Dlinux)

set(ark_SRCS
    main.cc
    arkwidget.cc
    arch.cc
    tar.cc
    zip.cc
    lha.cc
    ar.cc
    adddlg.cc
    extractdlg.cc
    arkprocess.cc
    karchive.cc
    text.c
)

set(ark_MOC
    arkwidget.h
    arch.h
    adddlg.h
    extractdlg.h
)

TQT3_WRAP_CPP(MOC_FILES ${ark_MOC})

add_executable(ark ${ark_SRCS} ${MOC_FILES})
target_link_libraries(ark
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    ${KDE1_KFM}
)

install(TARGETS ark RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES ark.xpm DESTINATION ${KDE1_ICONDIR})
install(FILES mini-ark.xpm DESTINATION ${KDE1_ICONDIR}/mini)
install(FILES ark.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)

file(GLOB ARK_HTML doc/*.html doc/*.gif)
install(FILES ${ARK_HTML} DESTINATION ${KDE1_HTMLDIR}/en/ark)
