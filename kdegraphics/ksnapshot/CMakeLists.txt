include_directories(${KDE1_INCLUDE_DIR})
add_definitions(-Dlinux)
# 上游 Makefile.am 的版本宏（configure 注入）
add_definitions(-DKSNAPVERSION="0.2.7")

set(ksnapshot_SRCS
    eps.cpp gif.cpp main.cpp formats.cpp ksnapshot.cpp preview.cpp
)
set(ksnapshot_MOC
    ksnapshot.h preview.h
)
TQT3_WRAP_CPP(MOC_FILES ${ksnapshot_MOC})

add_executable(ksnapshot1 ${ksnapshot_SRCS} ${MOC_FILES})
target_link_libraries(ksnapshot1
    ${TQT3_LIBRARIES} ${X11_LIBRARIES}
    ${KDE1_KDECORE} ${KDE1_KDEUI} ${KDE1_KIMGIO})

install(TARGETS ksnapshot1 RUNTIME DESTINATION ${KDE1_BINDIR})
install_icon(ksnapshot.xpm "${KDE1_ICONDIR}")
install(FILES mini-ksnapshot.xpm DESTINATION ${KDE1_ICONDIR}/mini RENAME ksnapshot.xpm)
install(FILES ksnapshot.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
