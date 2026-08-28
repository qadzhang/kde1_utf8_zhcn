include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(SAMBA_SRCS
    main.cpp bell.cpp kslidercontrol.cpp
)

set(SAMBA_MOC
    bell.h kslidercontrol.h
)
TQT3_WRAP_CPP(MOC_FILES ${SAMBA_MOC})

add_executable(kcmbell ${SAMBA_SRCS} ${MOC_FILES})
target_link_libraries(kcmbell
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kcmbell RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(samba "")
