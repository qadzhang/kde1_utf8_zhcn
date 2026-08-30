include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(SAMBA_SRCS
    main.cpp ksmbstatus.cpp
    )

set(SAMBA_MOC
    ksmbstatus.h
    )
TQT3_WRAP_CPP(MOC_FILES ${SAMBA_MOC})

add_executable(kcmsamba ${SAMBA_SRCS} ${MOC_FILES})
target_link_libraries(kcmsamba
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kcmsamba RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(samba "")
