include_directories(
    ${KDE1_INCLUDE_DIR}
    ${CURSES_INCLUDE_DIRS}
    )

set(KVT_SRCS command.c screen.c utmp.c
	xsetup.c debug.c resources.c main.C
	grkelot.c)

add_definitions(-DREAL_RESOURCES)
set(KVT_MOC main.h)

TQT3_WRAP_CPP(MOC_FILES ${KVT_MOC})

add_executable(kvt ${KVT_SRCS} ${MOC_FILES})
# [KDE1 Revival 2026] Xft/fontconfig（kvt UTF-8 渲染通道）
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(XFT xft)
endif()
if(NOT XFT_FOUND)
    find_path(XFT_INCLUDE_DIR X11/Xft/Xft.h)
    find_library(XFT_LIBRARY NAMES Xft)
    set(XFT_LIBRARIES ${XFT_LIBRARY})
    set(XFT_INCLUDE_DIRS ${XFT_INCLUDE_DIR})
endif()
include_directories(${XFT_INCLUDE_DIRS})

target_link_libraries(kvt
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${XFT_LIBRARIES}
    ${CURSES_LIBRARIES}
    )

install(TARGETS kvt RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(kvt.xpm "")
