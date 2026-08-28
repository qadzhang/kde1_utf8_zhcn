add_subdirectory(pics)

include_directories(
    ${KDE1_INCLUDE_DIR}
)

set(KBIFF_SRCS
    setupdlg.cpp kbiff.cpp main.cpp kbiffmonitor.cpp notify.cpp  Trace.cpp
    kbiffurl.cpp status.cpp
)

set(KBIFF_MOC
    kbiffmonitor.h setupdlg.h kbiff.h notify.h status.h
)

TQT3_WRAP_CPP(MOC_FILES ${KBIFF_MOC})

add_executable(kbiff ${KBIFF_SRCS} ${MOC_FILES})
target_link_libraries(kbiff
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
    ${KDE1_KFILE}
    ${KDE1_MEDIATOOL}
)

install(TARGETS kbiff RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kbiff.kdelnk DESTINATION ${KDE1_APPSDIR}/Internet)

install_icon(kbiff.xpm)
