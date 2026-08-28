include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KWMSOUND_SRCS kwmsound.C)

set(KWMSOUND_MOC kwmsound.h)

TQT3_WRAP_CPP(MOC_FILES ${KWMSOUND_MOC})

add_executable(kwmsound ${KWMSOUND_SRCS} ${MOC_FILES})
target_link_libraries(kwmsound
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_MEDIATOOL}
    )

install(TARGETS kwmsound RUNTIME DESTINATION ${KDE1_BINDIR})
