add_subdirectory(pics)

include_directories (
    ${KDE1_INCLUDE_DIR}
)

set(KMOON_SRCS  kmoon.cpp phases.cpp)

set(KMOON_MOC kmoon.h)

TQT3_WRAP_CPP(MOC_FILES ${KMOON_MOC})

add_executable(kmoon ${KMOON_SRCS} ${MOC_FILES})
target_link_libraries(kmoon
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
)

install(TARGETS kmoon RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kmoon.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)
install_icon(kmoon.xpm)
