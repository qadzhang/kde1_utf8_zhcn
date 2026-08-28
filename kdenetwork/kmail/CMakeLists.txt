add_subdirectory(pics)

include_directories(
    ${KDE1_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../mimelib/
)

set(KMAIL_SRCS
    charsetsDlg.cpp kalarmtimer.cpp kbusyptr.cpp kfileio.cpp kfontutils.cpp
    kmaccount.cpp kmacctfolder.cpp kmacctlocal.cpp kmacctmgr.cpp kmacctpop.cpp
    kmacctseldlg.cpp kmaddrbook.cpp kmaddrbookdlg.cpp kmcomposewin.cpp
    kmfilteraction.cpp kmfilter.cpp kmfilterdlg.cpp kmfiltermgr.cpp kmfolder.cpp
    kmfolderdia.cpp kmfolderdir.cpp kmfoldermgr.cpp kmfoldernode.cpp
    kmfolderseldlg.cpp kmfoldertree.cpp kmheaders.cpp kmidentity.cpp kmimemagic.cpp
    kmiostatusdlg.cpp kmmainwin.cpp kmmessage.cpp kmmsgbase.cpp kmmsginfo.cpp
    kmmsglist.cpp kmmsgpart.cpp kmmsgpartdlg.cpp kmnewiostatus.cpp
    kmnewiostatuswdg.cpp kmreaderwin.cpp kmsender.cpp kmsettings.cpp
    kmtopwidget.cpp kpgpbase.cpp kpgp.cpp main.cpp
)

set(KMAIL_MOC
    charsetsDlg.h kalarmtimer.h kbusyptr.h kmaccount.h
    kmacctmgr.h kmacctpop.h kmacctseldlg.h kmaddrbookdlg.h
    kmcomposewin.h kmfilteraction.h kmfilterdlg.h kmfolderdia.h
    kmfolderdir.h kmfolder.h kmfoldermgr.h kmfoldernode.h
    kmfolderseldlg.h kmfoldertree.h kmheaders.h kmiostatusdlg.h
    kmmainwin.h kmmsgbase.h kmmsgpartdlg.h kmnewiostatus.h
    kmnewiostatuswdg.h kmreaderwin.h kmsender.h kmsettings.h
    kmtopwidget.h kpgp.h
)

TQT3_WRAP_CPP(MOC_FILES ${KMAIL_MOC})

add_executable(kmail1 ${KMAIL_SRCS} ${MOC_FILES})
target_link_libraries(kmail1
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KHTMLW}
    ${KDE1_KFILE}
    ${KDE1_JSCRIPT}
    ${KDE1_KFM}
    ${KDE1_KIMGIO}
    ${KDE1_KSPELL}
    mimelib
)

install(TARGETS kmail1 RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(kmail.xpm)
install(FILES KMail.kdelnk DESTINATION ${KDE1_APPSDIR}/Internet)

install(FILES
    doc/logotp3.gif
    doc/index.html
    doc/index-1.html
    doc/index-2.html
    doc/index-3.html
    doc/index-4.html
    doc/index-5.html
    doc/index-6.html
    doc/index-7.html
    doc/index-8.html
    doc/index.html
DESTINATION ${KDE1_HTMLDIR}/en/kmail)
