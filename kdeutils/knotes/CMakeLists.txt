include_directories(
    ${KDE1_INCLUDE_DIR}
)

add_definitions(-Dlinux)

set(knotes_SRCS
    knotes.cpp
    configdlg.cpp
    fontdlg.cpp
    mail.cpp
    alarm.cpp
    bwdatetime.cpp
    spin.cpp
    timer.cpp
    savealarm.cpp
    renamedlg.cpp
    docking.cpp
)

set(knotes_MOC
    alarm.h
    configdlg.h
    docking.h
    fontdlg.h
    knotes.h
    mail.h
    renamedlg.h
    spin.h
)

TQT3_WRAP_CPP(MOC_FILES ${knotes_MOC})

add_executable(knotes1 ${knotes_SRCS} ${MOC_FILES})
target_link_libraries(knotes1
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
)

install(TARGETS knotes1 RUNTIME DESTINATION ${KDE1_BINDIR})
install_icon(knotes.xpm "${KDE1_ICONDIR}")
install_icon(knoteslogo.xpm "${KDE1_DATADIR}/knotes")
install(FILES knotes.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)

install(FILES
    docs/index.html
    docs/index-1.html
    docs/index-2.html
    docs/index-3.html
    docs/index-4.html
    docs/index-5.html
    docs/logotp3.gif
    DESTINATION ${KDE1_HTMLDIR}/en/knotes)
