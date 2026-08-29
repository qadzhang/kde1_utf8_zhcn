# ┌─ What : kppp(拨号网络工具)的 CMake 构建定义——2026-08 翻译自 Makefile.am
# │  Why  : 迁移 CMake 时未跟进被上层注释,现补齐接入(源清单/METASOURCES
# │         均取自 Makefile.am;当年安装含 setuid root 步骤,CMake 侧不做——
# │         现代系统拨号已由 NetworkManager 承担,kppp 作为历史功能保留)
# │  Who  : 上层 kdenetwork/CMakeLists.txt add_subdirectory(kppp)
# │  When : cmake 配置期生成构建规则
# │  Where: kdenetwork/kppp/CMakeLists.txt(样板:kbiff/CMakeLists.txt)
# │  How  : GUI 全家桶链接 kdeui+kfile;安装 bin/kdelnk
# ────────────────────────────────────────────────────────────────────
include_directories(
    ${KDE1_INCLUDE_DIR}
)

add_definitions(-Dlinux)

set(kppp_SRCS
    main.cpp
    accounts.cpp
    connect.cpp
    conwindow.cpp
    debug.cpp
    edit.cpp
    general.cpp
    iplined.cpp
    modem.cpp
    modemcmds.cpp
    pppdargs.cpp
    pppdata.cpp
    scriptedit.cpp
    pwentry.cpp
    modeminfo.cpp
    pppstatdlg.cpp
    pppstats.cpp
    miniterm.cpp
    accounting.cpp
    acctselect.cpp
    ruleset.cpp
    docking.cpp
    runtests.cpp
    loginterm.cpp
    ppplog.cpp
    log.cpp
    groupbox.cpp
    newwidget.cpp
    requester.cpp
    opener.cpp
    modemdb.cpp
)

set(kppp_MOC
    main.h
    accounting.h
    accounts.h
    acctselect.h
    connect.h
    conwindow.h
    debug.h
    docking.h
    edit.h
    general.h
    groupbox.h
    loginterm.h
    miniterm.h
    modem.h
    modemcmds.h
    modemdb.h
    modeminfo.h
    pppdargs.h
    pppstatdlg.h
    pwentry.h
    scriptedit.h
)

TQT3_WRAP_CPP(MOC_FILES ${kppp_MOC})

add_executable(kppp ${kppp_SRCS} ${MOC_FILES})
target_link_libraries(kppp
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    m
)

install(TARGETS kppp RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES Kppp.kdelnk DESTINATION ${KDE1_APPSDIR}/Internet)
