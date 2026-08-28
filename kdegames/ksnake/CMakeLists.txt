add_subdirectory(doc)

include_directories (
    ${KDE1_INCLUDE_DIR}
)

set(Ksnake_SRCS
    game.cpp trys.cpp rattler.cpp board.cpp
    level.cpp ball.cpp snake.cpp
    basket.cpp startroom.cpp lcdrange.cpp
    score.cpp pixServer.cpp progress.cpp levels.cpp
    view.cpp keys.cpp
)

set(Ksnake_MOC
    basket.h
    game.h
    keys.h
    lcdrange.h
    progress.h
    rattler.h
    score.h
    snake.h
    startroom.h
    trys.h
    view.h
    )

TQT3_WRAP_CPP(MOC_FILES ${Ksnake_MOC})

add_executable(ksnake ${Ksnake_SRCS} ${MOC_FILES} ${QwSpriteField_SOURCES})
target_link_libraries(ksnake
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS ksnake RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES ksnake.kdelnk DESTINATION ${KDE1_APPSDIR}/Games)
install_icon(ksnake.xpm)

file(GLOB BACKGROUNDS data/backgrounds/*.xpm)
install(FILES ${BACKGROUNDS}
    DESTINATION "${KDE1_DATADIR}/ksnake/backgrounds")

file(GLOB PIXMAPS data/pixmaps/*.xpm)
install(FILES ${PIXMAPS}
    DESTINATION "${KDE1_DATADIR}/ksnake/pics")

install(FILES data/highScores
    DESTINATION "${KDE1_DATADIR}/ksnake/")

file(GLOB LEVELS data/levels/room*)
install(FILES ${LEVELS}
    DESTINATION "${KDE1_DATADIR}/ksnake/levels")
