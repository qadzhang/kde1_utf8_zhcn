add_subdirectory(toolbar)

include_directories(
    ${KDE1_INCLUDE_DIR}
)

set(KFINGER_SRCS finger.C kfinger.C)

set(KFINGER_MOC finger.h kfinger.h)

TQT3_WRAP_CPP(MOC_FILES ${KFINGER_MOC})

add_executable(kfinger ${KFINGER_SRCS} ${MOC_FILES})
target_link_libraries(kfinger
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kfinger RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kfinger.kdelnk DESTINATION ${KDE1_APPSDIR}/Internet)
install(FILES kfingerrc DESTINATION ${KDE1_CONFDIR})
