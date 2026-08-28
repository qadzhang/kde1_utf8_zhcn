include_directories (
    ${KDE1_INCLUDE_DIR}
)

set(Ksirtet_SRCS
    gtetris.cpp tpiece.cpp spiece.cpp gtboard.cpp board.cpp
    net.cpp dialogs.cpp field.cpp main.cpp dbutton.cpp
)

set(Ksirtet_MOC
    spiece.h board.h dialogs.h field.h main.h
)

TQT3_WRAP_CPP(MOC_FILES ${Ksirtet_MOC})

add_executable(ksirtet ${Ksirtet_SRCS} ${MOC_FILES} ${QwSpriteField_SOURCES})
target_link_libraries(ksirtet
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS ksirtet RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES data/ksirtet.kdelnk DESTINATION ${KDE1_APPSDIR}/Games)
install_icon(data/ksirtet.xpm)

install(FILES
    data/index.html
    data/index-1.html
    data/index-2.html
    data/index-3.html
    data/index-4.html
    data/logotp3.gif
    DESTINATION ${KDE1_HTMLDIR}/en/ksirtet )
