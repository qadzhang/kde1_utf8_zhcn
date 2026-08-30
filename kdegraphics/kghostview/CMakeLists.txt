include_directories(${KDE1_INCLUDE_DIR})
add_definitions(-Dlinux)

set(kghostview_SRCS
    marklist.cpp copyright.cpp interpreter.cpp messages.cpp
    info.cpp print.cpp goto.cpp viewcontrol.cpp main.cpp kghostview.cpp
    kpswidget.cpp ps.c zoom.cpp
)
set(kghostview_MOC
    marklist.h copyright.h interpreter.h messages.h info.h print.h goto.h
    viewcontrol.h kghostview.h kpswidget.h zoom.h
)
TQT3_WRAP_CPP(MOC_FILES ${kghostview_MOC})

add_executable(kghostview1 ${kghostview_SRCS} ${MOC_FILES})
target_link_libraries(kghostview1
    ${TQT3_LIBRARIES} ${X11_LIBRARIES}
    ${KDE1_KDECORE} ${KDE1_KDEUI} ${KDE1_KFILE} ${KDE1_KFM})

install(TARGETS kghostview1 RUNTIME DESTINATION ${KDE1_BINDIR})
install_icon(kghostview.xpm "${KDE1_ICONDIR}")
install(FILES mini-kghostview.xpm DESTINATION ${KDE1_ICONDIR}/mini RENAME kghostview.xpm)
install(FILES kghostview.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
