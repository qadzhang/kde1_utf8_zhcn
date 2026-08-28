#   Modified for the KDE1 Revival Project, 2026
#   Maintainer: <维护者姓名> <邮箱>
#   Modifications written with GLM-5.3 (Z.ai)
#   （TQt3 底座迁移：QTableView 控件源接自 port/ 脚手架——TQt3 r14.1.6 已删
#     attic，实现搬运自 tqt3 r14.0.0，版权头保留于 port/src/qttableview.*）

include_directories(
    ${TQT3_INCLUDE_DIRS}
    ${KDELIBS_SOURCE_DIR}/kdecore
    ${KDELIBS_SOURCE_DIR}/kfile
    ${CMAKE_CURRENT_SOURCE_DIR}/../../port/src
    )

# port/src 是仓库级脚手架源目录（kdeui 承载其编译——KTreeList 等 KDE1 控件
# 直接继承 TQtTableView，进 kdeui 库最自然；显式化迁移完成后随之移除）
set(Q1SCAFFOLD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../port/src")

set(KDEUI_SRCS
        ${Q1SCAFFOLD_SRC}/qttableview.cpp
        kledlamp.cpp kprogress.cpp kpanner.cpp kcolordlg.cpp
		kselect.cpp kdatepik.cpp kdatetbl.cpp
		kfontdialog.cpp kmsgbox.cpp kpopmenu.cpp ktabctl.cpp
		ktreelist.cpp kstatusbar.cpp ktopwidget.cpp ktmainwindow.cpp
		kmenubar.cpp ktoolbar.cpp kbutton.cpp kslider.cpp
		kseparator.cpp klined.cpp kcombo.cpp krestrictedline.cpp
		kintegerline.cpp kspinbox.cpp kcontrol.cpp
		kcolorbtn.cpp kiconloaderdialog.cpp kwmmapp.cpp
		kbuttonbox.cpp ktablistbox.cpp knewpanner.cpp
		keditcl1.cpp keditcl2.cpp ktoolboxmgr.cpp kled.cpp
		kdbtn.cpp knotebook.cpp ktabbar.cpp kwizard.cpp kcontainer.cpp
		kkeydialog.cpp kurllabel.cpp kruler.cpp kquickhelp.cpp kcursor.cpp)

set(KDEUI_MOC
    kbutton.h kbuttonbox.h kcolorbtn.h
	kcolordlg.h kcombo.h kcontainer.h kcontrol.h kdatepik.h
	kdatetbl.h kdbtn.h keditcl.h kfontdialog.h
	kiconloaderdialog.h kintegerline.h kkeydialog.h kled.h
	kledlamp.h klined.h kmenubar.h kmsgbox.h knewpanner.h
	knotebook.h kpanner.h kpopmenu.h kprogress.h
	kquickhelp.h krestrictedline.h kruler.h kselect.h
	kslider.h kspinbox.h kstatusbar.h ktabbar.h ktabctl.h
	ktablistbox.h ktmainwindow.h ktoolbar.h ktoolboxmgr.h
	ktopwidget.h ktreelist.h kurllabel.h kwizard.h kwmmapp.h
	${Q1SCAFFOLD_SRC}/qttableview.h)

TQT3_WRAP_CPP(MOC_FILES ${KDEUI_MOC})

add_library(kdeui SHARED ${KDEUI_SRCS} ${MOC_FILES})
set_target_properties(kdeui PROPERTIES VERSION 2.0.0 SOVERSION 2)
target_link_libraries(kdeui kdecore ${TQT3_LIBRARIES} ${X11_LIBRARIES})
install(TARGETS kdeui LIBRARY DESTINATION ${KDE1_LIBRARIES})

install(FILES
        kledlamp.h kprogress.h kpanner.h kcolordlg.h kselect.h
		kdatepik.h kdatetbl.h
		kfontdialog.h kmsgbox.h kpopmenu.h ktabctl.h
		ktreelist.h kstatusbar.h ktopwidget.h ktmainwindow.h
		ktoolbar.h kmenubar.h kbutton.h kslider.h kseparator.h
		klined.h kcombo.h krestrictedline.h kintegerline.h
		kspinbox.h kcolorbtn.h kiconloaderdialog.h
		kwmmapp.h kbuttonbox.h ktablistbox.h kcontainer.h
		knewpanner.h kcontrol.h keditcl.h ktoolboxmgr.h kled.h
		kdbtn.h knotebook.h ktabbar.h kwizard.h kkeydialog.h
		kurllabel.h kruler.h kquickhelp.h kcursor.h
        DESTINATION include/kde1)

install(FILES
    error.xpm exclamation.xpm info.xpm stopsign.xpm question.xpm
    arrow_up.xbm arrow_down.xbm arrow_left.xbm arrow_right.xbm
    DESTINATION ${KDE1_DATADIR}/kde/pics)
