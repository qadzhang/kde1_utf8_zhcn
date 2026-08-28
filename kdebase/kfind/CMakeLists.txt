add_subdirectory(pics)

include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KFIND_SRCS
    kfarch.cpp kfdird.cpp kfindtop.cpp  kftabdlg.cpp
    kfsave.cpp kftypes.cpp kfoptions.cpp kfwin.cpp kfind.cpp
    main.cpp moc.cpp)

set(KFIND_MOC kfdird.h kfwin.h kfindtop.h
    kftabdlg.h kfoptions.h kfind.h)

TQT3_WRAP_CPP(MOC_FILES ${KFIND_MOC})

add_executable(kfind1 ${KFIND_SRCS} ${MOC_FILES})
target_link_libraries(kfind1
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
    ${KDE1_KFILE}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    )

install(TARGETS kfind1 RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(kfind.xpm "")
