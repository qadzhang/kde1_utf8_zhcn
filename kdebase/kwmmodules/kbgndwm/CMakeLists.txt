include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KBGNWM_SRCS main.cpp kbgndwm.cpp bg.cpp)

set(KBGNWM_MOC kbgndwm.h bg.h)

TQT3_WRAP_CPP(MOC_FILES ${KBGNWM_MOC})

add_executable(kbgndwm ${KBGNWM_SRCS} ${MOC_FILES})
target_link_libraries(kbgndwm
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KIMGIO}
    )

install(TARGETS kbgndwm RUNTIME DESTINATION ${KDE1_BINDIR})
