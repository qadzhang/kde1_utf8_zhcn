include_directories(
    ${KDE1_INCLUDE_DIR}
)

set(KNU_SRCS CommandDlg.cpp FingerDlg.cpp HostDlg.cpp knu.cpp
    MtrDlg.cpp OptionsDlg.cpp PingDlg.cpp TracerouteDlg.cpp
)

set(KNU_MOC CommandDlg.h FingerDlg.h HostDlg.h knu.h
    MtrDlg.h OptionsDlg.h PingDlg.h TracerouteDlg.h
)

TQT3_WRAP_CPP(MOC_FILES ${KNU_MOC})

add_executable(knu ${KNU_SRCS} ${MOC_FILES})
target_link_libraries(knu
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFILE}
    ${KDE1_KFM}
)

install(TARGETS knu RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES knu.kdelnk DESTINATION ${KDE1_APPSDIR}/Internet)
install_icon(knu.xpm)
