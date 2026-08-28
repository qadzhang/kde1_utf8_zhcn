include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KWMCCOM_SRCS kwmcom.C)

TQT3_WRAP_CPP(MOC_FILES ${KWMCCOM_MOC})

add_executable(kwmcom ${KWMCCOM_SRCS} ${MOC_FILES})
target_link_libraries(kwmcom
    ${KDE1_KDECORE}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    )

install(TARGETS kwmcom RUNTIME DESTINATION ${KDE1_BINDIR})
