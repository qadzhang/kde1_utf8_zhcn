include_directories(${KDE1_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-Dlinux)
# [KDE1 Revival 2026] 1999 C 源用 tentative definition 共享全局（gcc10 前
# 默认 -fcommon）；现代 gcc 默认 -fno-common 会多重定义链接错误
add_compile_options(-fcommon)
find_package(KPathsea REQUIRED)

# squeeze 工具：把 psheader.txt 压成 C 数组 psheader.c（上游 BUILT_SOURCES）
add_executable(squeeze squeeze.c)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/psheader.c
    COMMAND squeeze ${CMAKE_CURRENT_SOURCE_DIR}/psheader.txt ${CMAKE_CURRENT_BINARY_DIR}/psheader.c
    DEPENDS squeeze ${CMAKE_CURRENT_SOURCE_DIR}/psheader.txt VERBATIM)

set(kdvi_SRCS
    dviwin.cpp kdvi.cpp marklist.cpp prefs.cpp print.cpp
    printData.cpp printSetup.cpp printSetupData.cpp pushbutton.cpp
    scrbox.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/psheader.c dvi_draw.c dvi_init.c font-open.c gf.c new.c pk.c psgs.c
    special.c util.c vf.c
)
set(kdvi_MOC
    prefs.h printData.h scrbox.h kdvi.h printSetup.h print.h marklist.h
    dviwin.h pushbutton.h printSetupData.h
)
TQT3_WRAP_CPP(MOC_FILES ${kdvi_MOC})

add_executable(kdvi1 ${kdvi_SRCS} ${MOC_FILES})
target_link_libraries(kdvi1
    ${TQT3_LIBRARIES} ${X11_LIBRARIES} ${KPATHSEA_LIBRARY} m
    ${KDE1_KDECORE} ${KDE1_KDEUI} ${KDE1_KFILE} ${KDE1_KFM})

install(TARGETS kdvi1 RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kdvi.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
