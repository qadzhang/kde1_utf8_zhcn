include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KFONTMANAGER_SRCS
    main.cpp kfontmanager.cpp kfontlist.cpp kfontexplorer.cpp)
set(KFONTMANAGER_MOC kfontmanager.h kfontlist.h kfontexplorer.h)

TQT3_WRAP_CPP(MOC_FILES ${KFONTMANAGER_MOC})

add_executable(kfontmanager ${KFONTMANAGER_SRCS} ${MOC_FILES})
target_link_libraries(kfontmanager
    ${KDE1_KDECORE}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    m
    )

install(TARGETS kfontmanager RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(kfontmanager.xpm "")
