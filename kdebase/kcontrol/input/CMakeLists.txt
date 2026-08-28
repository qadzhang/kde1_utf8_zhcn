add_subdirectory(pics)
include_directories(
    ${TQT3_LIBRARIES}
    ${KDE1_INCLUDE_DIR}
    )

set(INPUT_SRCS
main.cpp mouse.cpp keyboard.cpp kslidercontrol.cpp
)

set(INPUT_MOC
mouse.h keyboard.h kslidercontrol.h
)
TQT3_WRAP_CPP(MOC_FILES ${INPUT_MOC})

add_executable(kcminput ${INPUT_SRCS} ${MOC_FILES})
target_link_libraries(kcminput
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kcminput RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(mouse.xpm "")
install_icon(keyboard.xpm "")
