include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${KDE1_INCLUDE_DIR}
    )


#install(FILES kimgio.h DESTINATION ${KDE1_INCLUDES})

set(KONSOLE_SRCS main.C schema.C session.C)

set(KONSOLE_MOC
    TEWidget.h
    TEmuVt102.h
    session.h
    main.h
    TEmulation.h
    TEShell.h)

TQT3_WRAP_CPP(MOC_FILES ${KONSOLE_MOC})

set(LIBKONSOLE_SRCS TEWidget.C TEmuVt102.C TEScreen.C TEmulation.C TEShell.C)
add_library(libkonsole STATIC ${LIBKONSOLE_SRCS} ${MOC_FILES})
target_link_libraries(libkonsole ${TQT3_LIBRARIES})
set_target_properties(libkonsole PROPERTIES CMAKE_EXE_LINKER_FLAGS "-version-info 9:1.9")
#set_target_properties(konsole PROPERTIES VERSION 0.1.9 SOVERSION 9)

add_executable(konsole1 ${KONSOLE_SRCS})
target_link_libraries(konsole1 libkonsole ${TQT3_LIBRARIES} ${X11_LIBRARIES} ${KDE1_KDECORE} ${KDE1_KIMGIO} ${KDE1_KDEUI})
install(TARGETS konsole1 RUNTIME DESTINATION ${KDE1_BINDIR})

TQT3_WRAP_CPP(KCMKONSOLE_MOC_FILES kcmkonsole.h)
add_executable(kcmkonsole1 kcmkonsole.C schema.C ${KCMKONSOLE_MOC_FILES})
target_link_libraries(kcmkonsole1 ${TQT3_LIBRARIES} ${KDE1_KDECORE} ${KDE1_KIMGIO} ${KDE1_KDEUI})
install(TARGETS kcmkonsole1 RUNTIME DESTINATION ${KDE1_BINDIR})
