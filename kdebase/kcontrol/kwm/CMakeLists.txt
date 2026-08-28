add_subdirectory(pics)
include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KCMKWM_SRCS
    main.cpp
    titlebar.cpp
    windows.cpp
    desktop.cpp
    mouse.cpp
    advanced.cpp
)

set(KCMKWM_MOC
    advanced.h
    desktop.h
    mouse.h
    titlebar.h
    windows.h
)
TQT3_WRAP_CPP(MOC_FILES ${KCMKWM_MOC})

add_executable(kcmkwm ${KCMKWM_SRCS} ${MOC_FILES})
target_link_libraries(kcmkwm
    ${TQT3_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kcmkwm RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(kwm "")
