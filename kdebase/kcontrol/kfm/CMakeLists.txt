include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KFM_SRCS
main.cpp htmlopts.cpp miscopts.cpp  kproxydlg.cpp khttpoptdlg.cpp useragentdlg.cpp kcookiesdlg.cpp rootopts.cpp
)

set(KFM_MOC
htmlopts.h miscopts.h  kproxydlg.h khttpoptdlg.h useragentdlg.h kcookiesdlg.h rootopts.h
)
TQT3_WRAP_CPP(MOC_FILES ${KFM_MOC})

add_executable(kcmkfm ${KFM_SRCS} ${MOC_FILES})
target_link_libraries(kcmkfm
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kcmkfm RUNTIME DESTINATION ${KDE1_BINDIR})
