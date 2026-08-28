add_subdirectory(docs)
add_subdirectory(pics)

include_directories (
    ${KDE1_INCLUDE_DIR}
)

set(KONQUEST_SRCS Konquest.cc gameboard.cc gamecore.cc int_validator.cc
    mainwin.cc map_widget.cc minimap.cc newgamedlg.cc planet_info.cc
    gameenddlg.cc  scoredlg.cc)

set(KONQUEST_MOC gameboard.h gamecore.h int_validator.h mainwin.h
    map_widget.h minimap.h newgamedlg.h planet_info.h scoredlg.h
    gameenddlg.h)

TQT3_WRAP_CPP(MOC_FILES ${KONQUEST_MOC})

add_executable(konquest1 ${KONQUEST_SRCS} ${MOC_FILES})
target_link_libraries(konquest1
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS konquest1 RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES Konquest.kdelnk DESTINATION ${KDE1_APPSDIR}/Games)
install_icon(konquest.xpm)
