add_subdirectory(icons)

include_directories(
    ${KDE1_INCLUDE_DIR}
    )

set(KMENUEDIT_SRCS
kmenuedit.cpp main.cpp pmenu.cpp
	cpopmenu.cpp confmenu.cpp button.cpp IconPathDialog.cpp
	IconPathDialogData.cpp DesktopPathDialog.cpp
	DesktopPathDialogData.cpp entrydialog.cpp
	MenuNameDialog.cpp MenuNameDialogData.cpp )

set(KMENUEDIT_MOC
kmenuedit.h pmenu.h cpopmenu.h confmenu.h
	button.h IconPathDialog.h IconPathDialogData.h
	DesktopPathDialog.h DesktopPathDialogData.h
	entrydialog.h	MenuNameDialog.h MenuNameDialogData.h)

TQT3_WRAP_CPP(MOC_FILES ${KMENUEDIT_MOC})

add_executable(kmenuedit1 ${KMENUEDIT_SRCS} ${MOC_FILES})
target_link_libraries(kmenuedit1
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
    ${KDE1_KFILE}
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    )

install(TARGETS kmenuedit1 RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(kmenuedit.xpm "")
