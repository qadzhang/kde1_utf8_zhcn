include_directories(${KDE1_INCLUDE_DIR})
add_definitions(-Dlinux)

set(kiconedit_SRCS
    utils.cpp main.cpp kiconedit.cpp kicongrid.cpp kiconcolors.cpp kcolorgrid.cpp
    kiconfilebase.cpp kiconfiledlg.cpp kiconcombiview.cpp kiconfileview.cpp kicondetaillist.cpp
    kicontools.cpp kicon.cpp kresize.cpp knew.cpp debug.cpp properties.cpp
    kiconeditslots.cpp kiconconfig.cpp kdropsite.cpp kdragsource.cpp
)
set(kiconedit_MOC
    kiconedit.h kicongrid.h kiconcolors.h kcolorgrid.h kiconfilebase.h
    kiconfiledlg.h kiconcombiview.h kiconfileview.h kicondetaillist.h
    kicontools.h kicon.h kresize.h knew.h properties.h kiconconfig.h
    kdropsite.h kdragsource.h main.h
)
TQT3_WRAP_CPP(MOC_FILES ${kiconedit_MOC})

add_executable(kiconedit1 ${kiconedit_SRCS} ${MOC_FILES})
target_link_libraries(kiconedit1
    ${TQT3_LIBRARIES} ${X11_LIBRARIES}
    ${KDE1_KDECORE} ${KDE1_KDEUI} ${KDE1_KFILE} ${KDE1_KFM} ${KDE1_KIMGIO})

install(TARGETS kiconedit1 RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kiconedit.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
install(FILES pics/kiconedit.xpm DESTINATION ${KDE1_ICONDIR})
install(FILES pics/mini/kiconedit.xpm DESTINATION ${KDE1_ICONDIR}/mini RENAME kiconedit.xpm)
