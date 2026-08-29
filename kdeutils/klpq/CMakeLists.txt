# ┌─ What : klpq(打印队列查看器)的 CMake 构建定义——2026-08 翻译自 Makefile.am
# │  Why  : 迁移 CMake 时未跟进被上层注释,现补齐接入(源清单取自
# │         Makefile.am 的 klpq_SOURCES;该应用用 -ktranslate 提取翻译)
# │  Who  : 上层 kdeutils/CMakeLists.txt add_subdirectory(klpq)
# │  When : cmake 配置期生成构建规则
# │  Where: kdeutils/klpq/CMakeLists.txt(样板:kjots/CMakeLists.txt)
# │  How  : 链接 kdecore/kdeui/kfile;安装 bin/图标/kdelnk
# ────────────────────────────────────────────────────────────────────
include_directories(
    ${KDE1_INCLUDE_DIR}
)

add_definitions(-Dlinux)

set(klpq_SRCS
    main.cpp
    klpq.cpp
    ConfAutoUpdate.cpp
    ConfAutoUpdateData.cpp
    krowtable.cpp
    kheader.cpp
    queueview.cpp
    klpqspooler.cpp
    FirstStart.cpp
    FirstStartData.cpp
    SpoolerConfig.cpp
    SpoolerConfigData.cpp
)

set(klpq_MOC
    klpq.h
    ConfAutoUpdate.h
    krowtable.h
    kheader.h
    queueview.h
    FirstStart.h
    FirstStartData.h
    SpoolerConfig.h
    SpoolerConfigData.h
)

TQT3_WRAP_CPP(MOC_FILES ${klpq_MOC})

add_executable(klpq1 ${klpq_SRCS} ${MOC_FILES})
target_link_libraries(klpq1
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    ${KDE1_KFM}
)

install(TARGETS klpq1 RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES klpq.xpm DESTINATION ${KDE1_ICONDIR})
install(FILES mini-klpq.xpm DESTINATION ${KDE1_ICONDIR}/mini)
install(FILES KLpq.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)
