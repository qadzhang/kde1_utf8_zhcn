# ┌─ What : kab(KDE 地址簿)的 CMake 构建定义——2026-08 翻译自 Makefile.am
# │  Why  : 迁移 CMake 时未跟进被上层注释,现补齐接入(kabapi_test 为开发期
#         测试程序,不随交付构建;源清单取自 Makefile.am 的 kab_SOURCES)
# │  Who  : 上层 kdeutils/CMakeLists.txt add_subdirectory(kab)
# │  When : cmake 配置期生成构建规则
# │  Where: kdeutils/kab/CMakeLists.txt(样板:kjots/CMakeLists.txt)
# │  How  : 链接 kdelibs 的 libkab(KDE1_KAB)+ kfile;安装 bin/图标/kdelnk
# ────────────────────────────────────────────────────────────────────
include_directories(
    ${KDE1_INCLUDE_DIR}
)

add_definitions(-Dlinux -DNDEBUG)  # [2026-08-31] 生产构建关闭 kab 断言（debug.h 的 CHECK/REQUIRE/ENSURE 本为调试构建准备；空库启动断言恒假导致用户看到"发现了一个bug"对话框）

set(kab_SRCS
    main.cc
    AddressBookAbout.cpp
    AddressBookAboutData.cpp
    toplevelwidget.cc
)

set(kab_MOC
    toplevelwidget.h
    AddressBookAboutData.h
    AddressBookAbout.h
)

TQT3_WRAP_CPP(MOC_FILES ${kab_MOC})

# libkab 链接:优先仓库 staging 暂存区(开发期刚构建的同源库),回退系统
# 安装路径(/usr/kde1/lib 里可能残留 Qt1 时代的旧同名库,符号不兼容)
find_library(KAB_LIBRARY kab
    ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib
    ${KDE1_LIBRARIES})

add_executable(kab1 ${kab_SRCS} ${MOC_FILES})
target_link_libraries(kab1
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KAB_LIBRARY}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    ${KDE1_KIMGIO}
    ${KDE1_KFM}
)

install(TARGETS kab1 RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kab.xpm DESTINATION ${KDE1_ICONDIR})
install(FILES mini-kab.xpm DESTINATION ${KDE1_ICONDIR}/mini)
install(FILES kab.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)
