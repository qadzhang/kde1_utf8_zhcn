include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KSTART_SRCS kstart.C)

set(KSTART_MOC kstart.h)

TQT3_WRAP_CPP(MOC_FILES ${KSTART_MOC})

add_executable(kstart ${KSTART_SRCS} ${MOC_FILES})
target_link_libraries(kstart
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    )

install(TARGETS kstart RUNTIME DESTINATION ${KDE1_BINDIR})

