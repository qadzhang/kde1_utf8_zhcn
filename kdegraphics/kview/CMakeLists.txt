include_directories(${KDE1_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/layout)
add_definitions(-Dlinux)

# layout 静态库（baglayout/bagconst）
set(layout_SRCS layout/bagconst.cpp layout/baglayout.cpp layout/typolayout.cpp)
# [KDE1 Revival 2026] 传绝对路径：TQT3_WRAP_CPP 的 -p 前缀按头自身目录
# 解析 include（相对名会把前缀错指到 kview/ 顶层）
set(layout_MOC \${CMAKE_CURRENT_SOURCE_DIR}/layout/baglayout.h \${CMAKE_CURRENT_SOURCE_DIR}/layout/typolayout.h)
TQT3_WRAP_CPP(layout_MOC_FILES ${layout_MOC})
add_library(kviewlayout STATIC ${layout_SRCS} ${layout_MOC_FILES})

set(kview_SRCS
    canvas.cpp kview.cpp main.cpp viewer.cpp
    filter.cpp filtlist.cpp colour.cpp filtmenu.cpp ilistdlg.cpp
    numdlg.cpp kcproc.cpp khelpidx.cpp prefdlg.cpp kaccelmenuwatch.cpp
)
set(kview_MOC
    canvas.h kview.h viewer.h filter.h filtlist.h colour.h filtmenu.h
    ilistdlg.h numdlg.h kcproc.h khelpidx.h prefdlg.h kaccelmenuwatch.h
)
TQT3_WRAP_CPP(MOC_FILES ${kview_MOC})

add_executable(kview1 ${kview_SRCS} ${MOC_FILES})
target_link_libraries(kview1 kviewlayout
    ${TQT3_LIBRARIES} ${KDE1_KFM} ${X11_LIBRARIES}
    ${KDE1_KDECORE} ${KDE1_KDEUI} ${KDE1_KFILE} ${KDE1_KIMGIO})

install(TARGETS kview1 RUNTIME DESTINATION ${KDE1_BINDIR})
install_icon(kview.xpm "${KDE1_ICONDIR}")
install(FILES mini-kview.xpm DESTINATION ${KDE1_ICONDIR}/mini RENAME kview.xpm)
install(FILES kview.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
