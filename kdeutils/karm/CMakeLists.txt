# ┌─ What : karm(计时任务管理器)的 CMake 构建定义——2026-08 翻译自 Makefile.am
# │  Why  : 迁移 CMake 时未跟进被上层注释,现补齐接入(源清单取自
# │         Makefile.am 的 karm_SOURCES;AUTOMOC 时代全部含 Q_OBJECT 的头入 MOC)
# │  Who  : 上层 kdeutils/CMakeLists.txt add_subdirectory(karm)
# │  When : cmake 配置期生成构建规则
# │  Where: kdeutils/karm/CMakeLists.txt(样板:kjots/CMakeLists.txt)
# │  How  : 链接 kdecore/kdeui/kfile;安装 bin/图标/support 下的 kdelnk
# ────────────────────────────────────────────────────────────────────
include_directories(
    ${KDE1_INCLUDE_DIR}
)

add_definitions(-Dlinux)

set(karm_SRCS
    main.cpp
    karm.cpp
    task.cpp
    top.cpp
    adddata.cpp
    adddlg.cpp
    kaccelmenuwatch.cpp
)

set(karm_MOC
    karm.h
    task.h
    top.h
    adddata.h
    adddlg.h
    kaccelmenuwatch.h
)

TQT3_WRAP_CPP(MOC_FILES ${karm_MOC})

add_executable(karm1 ${karm_SRCS} ${MOC_FILES})
target_link_libraries(karm1
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
)

install(TARGETS karm1 RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES karm.xpm DESTINATION ${KDE1_ICONDIR})
install(FILES mini-karm.xpm DESTINATION ${KDE1_ICONDIR}/mini)
install(FILES support/KArm.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)
