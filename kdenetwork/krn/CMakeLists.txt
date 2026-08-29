# ┌─ What : krn(新闻阅读器)的 CMake 构建定义——2026-08 翻译自 Makefile.am
# │  Why  : 迁移 CMake 时未跟进被上层注释,现补齐接入(源清单/METASOURCES 取自
# │         Makefile.am;当年三程序 krn/kdecode/newkrn 中只建主程序 krn——
# │         kdecode 为独立解码工具、newkrn 为未完成的实验界面,均无 kdelnk)
# │  Who  : 上层 kdenetwork/CMakeLists.txt add_subdirectory(krn)
# │  When : cmake 配置期生成构建规则
# │  Where: kdenetwork/krn/CMakeLists.txt(样板:kmail/CMakeLists.txt)
# │  How  : 依赖 kspell/khtmlw/kfile/mimelib/gdbm;各 cpp 已自带 moc include
# ────────────────────────────────────────────────────────────────────
include_directories(
    ${KDE1_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../kmail  # krn 借用 kmail 的 km* 头(KMFolder 仅作指针形参)
)

add_definitions(-Dlinux -DKRN)  # Makefile.am 的 KRN 宏:krn 版 km* 代码的分支开关(aboutText/KMMainWin 绕过等)

set(krn_SRCS
    main.cpp
    NNTP.cpp
    groupdlg.cpp
    artdlg.cpp
    newsgroup.cpp
    asker.cpp
    identDlg.cpp
    NNTPConfigDlg.cpp
    rmbpop.cpp
    aboutDlg.cpp
    ktempfile.cpp
    kscorer.cpp
    fontsDlg.cpp
    kmmessage.cpp
    kmmsgpart.cpp
    kmmsginfo.cpp
    kmcomposewin.cpp
    kmimemagic.cpp
    kmsender.cpp
    findArtDlg.cpp
    kmmsgbase.cpp
    kmidentity.cpp
    kfileio.cpp
    kmmsgpartdlg.cpp
    kbusyptr.cpp
    kalarmtimer.cpp
    kmreaderwin.cpp
    kpgp.cpp
    krnsender.cpp
    kmaddrbookdlg.cpp
    kmaddrbook.cpp
    ktreeview.cpp
    expireDlg.cpp
    tlform.cpp
    tlwidget.cpp
    typelayout.cpp
    printdlg.cpp
    rulesDlg.cpp
    sortDlg.cpp
    rules.cpp
    charsetsDlg.cpp
    expirestatusdlg.cpp
    kfontutils.cpp
    kmtopwidget.cpp
    kpgpbase.cpp
    TooManydlg.cpp
)

set(krn_MOC
    groupdlg.h
    NNTP.h
    artdlg.h
    asker.h
    identDlg.h
    NNTPConfigDlg.h
    rmbpop.h
    aboutDlg.h
    fontsDlg.h
    kmcomposewin.h
    findArtDlg.h
    kmmsgpartdlg.h
    kbusyptr.h
    kalarmtimer.h
    kmreaderwin.h
    kpgp.h
    krnsender.h
    kmsender.h
    kmaddrbookdlg.h
    ktreeview.h
    expireDlg.h
    printdlg.h
    rulesDlg.h
    sortDlg.h
    charsetsDlg.h
    expirestatusdlg.h
    kmtopwidget.h
    TooManydlg.h
)

TQT3_WRAP_CPP(MOC_FILES ${krn_MOC})

# libgdbm:系统仅有运行库(libgdbm.so.6,无 .so 开发链接)时直接链接运行库
find_library(GDBM_LIBRARY gdbm
    /usr/lib/x86_64-linux-gnu
    /usr/lib64)
if (NOT GDBM_LIBRARY)
    set(GDBM_LIBRARY /usr/lib/x86_64-linux-gnu/libgdbm.so.6)
endif ()

# 系统路径 /usr/kde1/lib 有 Qt1 时代同名旧库,优先链接仓库 staging 的同源新库
find_library(KRN_KHTMLW khtmlw ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KRN_KSPELL kspell ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KRN_KFILE kfile ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KRN_KFM kfm ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KRN_JSCRIPT jscript ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KRN_KIMGIO kimgio ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})

add_executable(krn ${krn_SRCS} ${MOC_FILES})
target_link_libraries(krn
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KRN_KFILE}
    ${KRN_KSPELL}
    ${KRN_KHTMLW}
    ${KRN_KFM}
    ${KRN_JSCRIPT}
    ${KRN_KIMGIO}
    mimelib
    ${GDBM_LIBRARY}
)

install(TARGETS krn RUNTIME DESTINATION ${KDE1_BINDIR})
