# [KDE1 Revival 2026] SNI 托盘 host 的 DBus 依赖
find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS1 REQUIRED dbus-1)

add_subdirectory(pics)
add_subdirectory(sample_kdelnk_files)

include_directories(
    ${KDE1_INCLUDE_DIR}
    ${DBUS1_INCLUDE_DIRS}
    )

set(KPANEL_SRCS  kpanel.C props.C layout.C tools.C main.C events.C pmenu.C pfilemenu.C snitray.cpp)

set(KPANEL_MOC kpanel.h pmenu.h snitray.h)

TQT3_WRAP_CPP(MOC_FILES ${KPANEL_MOC})

add_executable(kpanel ${KPANEL_SRCS} ${MOC_FILES})
target_link_libraries(kpanel
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
     ${DBUS1_LIBRARIES})

install(TARGETS kpanel RUNTIME DESTINATION ${KDE1_BINDIR})

install(FILES personal.kdelnk DESTINATION ${KDE1_DATADIR}/kpanel/default RENAME personal_directory)
install(FILES Home.kdelnk Desktop.kdelnk DESTINATION ${KDE1_DATADIR}/kpanel/default)
