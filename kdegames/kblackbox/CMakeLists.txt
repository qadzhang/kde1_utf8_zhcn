add_subdirectory(doc)
add_subdirectory(pics)

include_directories (
    ${KDE1_INCLUDE_DIR}
)

set(KBLACKBOX_SRCS kbbgame.cpp kbbgfx.cpp main.cpp util.cpp)

set(KBLACKBOX_MOC kbbgame.h kbbgfx.h)

TQT3_WRAP_CPP(MOC_FILES ${KBLACKBOX_MOC})

add_executable(kblackbox1 ${KBLACKBOX_SRCS} ${MOC_FILES})
target_link_libraries(kblackbox1
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kblackbox1 RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kblackbox.kdelnk DESTINATION ${KDE1_APPSDIR}/Games)
install_icon(kblackbox.xpm)
