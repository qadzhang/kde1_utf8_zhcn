include_directories(${KDE1_INCLUDE_DIR})
add_definitions(-Dlinux)

set(kpaint_SRCS
    main.cpp app.cpp canvas.cpp kpaint.cpp manager.cpp
    formats.cpp infodialog.cpp palette.cpp palettedialog.cpp
    formatdialog.cpp canvassize.cpp properties.cpp gif.cpp
    QwViewport.cpp tool.cpp ellipse.cpp circle.cpp line.cpp
    spraycan.cpp rectangle.cpp pen.cpp areaselect.cpp depthdialog.cpp
    roundangle.cpp navwidget.cpp sidebar.cpp view.cpp colorbar.cpp
    mainview.cpp text.cpp
)
set(kpaint_MOC
    app.h canvas.h kpaint.h manager.h infodialog.h palette.h palettedialog.h
    formatdialog.h canvassize.h properties.h QwViewport.h tool.h ellipse.h
    circle.h line.h spraycan.h rectangle.h pen.h areaselect.h depthdialog.h
    roundangle.h navwidget.h sidebar.h view.h colorbar.h mainview.h text.h
)
TQT3_WRAP_CPP(MOC_FILES ${kpaint_MOC})

add_executable(kpaint1 ${kpaint_SRCS} ${MOC_FILES})
target_link_libraries(kpaint1
    ${TQT3_LIBRARIES} ${X11_LIBRARIES} gif
    ${KDE1_KDECORE} ${KDE1_KDEUI} ${KDE1_KFILE} ${KDE1_KFM} ${KDE1_KIMGIO})

install(TARGETS kpaint1 RUNTIME DESTINATION ${KDE1_BINDIR})
install_icon(kpaint.xpm "${KDE1_ICONDIR}")
install(FILES mini-kpaint.xpm DESTINATION ${KDE1_ICONDIR}/mini RENAME kpaint.xpm)
install(FILES kpaint.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)

# [KDE1 Revival 2026] 工具工具栏图标安装：1999 年 Makefile.am 的安装规则
# 在 CMake 化时遗漏——line/rectangle/spraycan 等 XPM 从未进包，运行期
# new QPixmap(绝对路径) 得空图，工具栏全是乱色块。largetext.xpm 取自
# kdvi/pix（text.cpp 文本工具图标，KDE1 时代该文件从未被安装过）
install(FILES
    toolpics/circle.xpm toolpics/areaselect.xpm toolpics/line.xpm
    toolpics/pen.xpm toolpics/spraycan.xpm toolpics/ellipse.xpm
    toolpics/rectangle.xpm toolpics/roundangle.xpm toolpics/editpaste.xpm
    toolpics/filenew.xpm toolpics/editcopy.xpm toolpics/fileopen.xpm
    toolpics/editcut.xpm toolpics/filefloppy.xpm
    "toolpics/viewmag+.xpm" "toolpics/viewmag-.xpm"
    DESTINATION ${KDE1_DATADIR}/kpaint/toolbar)
install(FILES ../kdvi/pix/largetext.xpm
    DESTINATION ${KDE1_DATADIR}/kpaint/toolbar)
