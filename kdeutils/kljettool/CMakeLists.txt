# ┌─ What : kljettool(HP LaserJet 打印机工具)的 CMake 构建定义——2026-08 翻译自 Makefile.am
# │  Why  : 迁移 CMake 时未跟进被上层注释,现补齐接入(源清单取自 Makefile.am;
# │         其 LIB_KDECORE-only 链接在 CMake 侧对应 kdecore+kdeui+kfile 全家桶)
# │  Who  : 上层 kdeutils/CMakeLists.txt add_subdirectory(kljettool)
# │  When : cmake 配置期生成构建规则
# │  Where: kdeutils/kljettool/CMakeLists.txt(样板:kjots/CMakeLists.txt)
# │  How  : SRCS/MOC 照 Makefile.am;docs 子树为纯 HTML 文档不再挂载
# ────────────────────────────────────────────────────────────────────
include_directories(
    ${KDE1_INCLUDE_DIR}
)

add_definitions(-Dlinux)

set(kljettool_SRCS
    main.cpp
    Paper.cpp
    PaperData.cpp
    Fonts.cpp
    FontsData.cpp
    Printer.cpp
    PrinterData.cpp
    Misc.cpp
    MiscData.cpp
    Data.cpp
    about.cpp
)

set(kljettool_MOC
    main.h
    Paper.h
    PaperData.h
    Fonts.h
    FontsData.h
    Printer.h
    PrinterData.h
    Misc.h
    MiscData.h
    Data.h
    about.h
)

TQT3_WRAP_CPP(MOC_FILES ${kljettool_MOC})

add_executable(kljettool ${kljettool_SRCS} ${MOC_FILES})
target_link_libraries(kljettool
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    m
)

install(TARGETS kljettool RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kljettool.xpm DESTINATION ${KDE1_ICONDIR})
install(FILES mini-kljettool.xpm DESTINATION ${KDE1_ICONDIR}/mini)
install(FILES KLJetTool.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)
