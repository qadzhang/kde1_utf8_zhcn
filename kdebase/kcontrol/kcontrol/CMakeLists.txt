include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${KDE1_INCLUDE_DIR}
    )

add_subdirectory(pics)
set(KCONTROL_SRCS
configlist.cpp main.cpp toplevel.cpp kdelnk.cpp mainwidget.cpp kswallow.cpp
)
set(KCONTROL_MOC toplevel.h kdelnk.h mainwidget.h configlist.h kswallow.h)

TQT3_WRAP_CPP(MOC_FILES ${KCONTROL_MOC})

add_executable(kcontrol ${KCONTROL_SRCS} ${MOC_FILES})
target_link_libraries(kcontrol
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kcontrol RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(kcontrol.xpm "")
