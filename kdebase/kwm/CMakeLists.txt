add_subdirectory(pics)

include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KWM_SRCS client.C manager.C main.C minicli.C warning.C drag.C
    logout.C taskmgr.C kURLcompletion.cpp gradientFill.C )

set( METASOURCES client.h manager.h main.h minicli.h
    warning.h logout.h taskmgr.h kURLcompletion.h )

TQT3_WRAP_CPP(MOC_SRCS ${METASOURCES})

add_executable(kwm ${KWM_SRCS} ${MOC_SRCS})
target_link_libraries(kwm 
    ${KDE1_KDECORE}
    ${KDE1_KHTMLW}
    ${KDE1_KFILE}
    ${KDE1_KIMGIO}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
    ${KDE1_JSCRIPT}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    )

install(TARGETS kwm RUNTIME DESTINATION ${KDE1_BINDIR})
install_icon(kwm.xpm "")
