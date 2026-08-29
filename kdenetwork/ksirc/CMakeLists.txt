# ┌─ What : ksirc(IRC 客户端)全家桶的 CMake 构建定义——2026-08 翻译自 Makefile.am
# │  Why  : 迁移 CMake 时未跟进被上层注释,现补齐接入。ksirc 工程为
# │         「主程序 + 7 静态子库 + puke 动态库 + pws 密码件 + 独立 ksticker」;
# │         Makefile.am 亦编 ksircgetdate(yacc 产物,仓库已含 getdate.c)
# │  Who  : 上层 kdenetwork/CMakeLists.txt add_subdirectory(ksirc)
# │  When : cmake 配置期生成构建规则
# │  Where: kdenetwork/ksirc/CMakeLists.txt(样板:kmail/CMakeLists.txt)
# │  How  : ① 先建各静态库(kplunger/KSProgress/KSTicker/KSCutDialog/KSPrefs/
# │         KSColourPicker/KSOpenkSirc/pws)→ ② puke 为 SHARED(ksirc 运行期
# │         dlopen 需要)→ ③ 主程序与 ksticker 链全部;各 Q_OBJECT 头经
# │         TQT3_WRAP_CPP 生成 .moc,由源内显式 include 并入编译单元
# ────────────────────────────────────────────────────────────────────
include_directories(
    ${KDE1_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/kplunger
)
# 系统路径 /usr/kde1/lib 有 Qt1 时代同名旧库,优先链接仓库 staging 同源新库
find_library(KSIRC_KFILE kfile ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})

add_definitions(-Dlinux -DKSIRC_APP)

# ── 静态子库(当年 ./sub/lib*.a,等价 STATIC 库);
#    各库先声明 moc 需求(TQT3_WRAP_CPP),库源里的显式 include "*.moc" 才有产物 ──
TQT3_WRAP_CPP(KPLUNGER_MOC kplunger/kplunger.h)
add_library(kplunger STATIC kplunger/new.cpp ${KPLUNGER_MOC})

TQT3_WRAP_CPP(KSPROGRESS_MOC ${CMAKE_CURRENT_SOURCE_DIR}/KSProgress/ksprogress.h ${CMAKE_CURRENT_SOURCE_DIR}/KSProgress/ksprogressdata.h)
add_library(ksprogress STATIC
    KSProgress/ksprogress.cpp KSProgress/ksprogressdata.cpp ${KSPROGRESS_MOC})

TQT3_WRAP_CPP(KSTICKERLIB_MOC ${CMAKE_CURRENT_SOURCE_DIR}/KSTicker/ksticker.h ${CMAKE_CURRENT_SOURCE_DIR}/KSTicker/speeddialog.h ${CMAKE_CURRENT_SOURCE_DIR}/KSTicker/speeddialogData.h)
add_library(kstickerlib STATIC
    KSTicker/ksticker.cpp KSTicker/speeddialog.cpp KSTicker/speeddialogData.cpp ${KSTICKERLIB_MOC})

TQT3_WRAP_CPP(KSCUTDIALOG_MOC ${CMAKE_CURRENT_SOURCE_DIR}/KSCutDialog/KSCutDialog.h ${CMAKE_CURRENT_SOURCE_DIR}/KSCutDialog/KSCutDialogData.h ${CMAKE_CURRENT_SOURCE_DIR}/KSCutDialog/cutmle.h)
add_library(kscutdialog STATIC
    KSCutDialog/KSCutDialog.cpp KSCutDialog/KSCutDialogData.cpp KSCutDialog/cutmle.cpp ${KSCUTDIALOG_MOC})

TQT3_WRAP_CPP(KSPREFS_MOC
    ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/ksprefs.h ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/general.h ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/generalData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/startup.h ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/startupdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/serverchannel.h ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/serverchanneldata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/UserMenuRef.h ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/UserMenuRefData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/defaultfilters.h ${CMAKE_CURRENT_SOURCE_DIR}/KSPrefs/defaultfiltersdata.h)
add_library(ksprefs STATIC
    KSPrefs/ksprefs.cpp
    KSPrefs/general.cpp KSPrefs/generalData.cpp
    KSPrefs/startup.cpp KSPrefs/startupdata.cpp
    KSPrefs/serverchannel.cpp KSPrefs/serverchanneldata.cpp
    KSPrefs/UserMenuRef.cpp KSPrefs/UserMenuRefData.cpp
    KSPrefs/defaultfilters.cpp KSPrefs/defaultfiltersdata.cpp ${KSPREFS_MOC})

TQT3_WRAP_CPP(KSCOLOURPICKER_MOC ${CMAKE_CURRENT_SOURCE_DIR}/KSColourPicker/kscolourpicker.h ${CMAKE_CURRENT_SOURCE_DIR}/KSColourPicker/kscolourpickerData.h)
add_library(kscolourpicker STATIC
    KSColourPicker/kscolourpicker.cpp KSColourPicker/kscolourpickerData.cpp ${KSCOLOURPICKER_MOC})

TQT3_WRAP_CPP(KSOPENKSIRC_MOC
    ${CMAKE_CURRENT_SOURCE_DIR}/KSOpenkSirc/open_ksirc.h ${CMAKE_CURRENT_SOURCE_DIR}/KSOpenkSirc/open_ksircData.h ${CMAKE_CURRENT_SOURCE_DIR}/KSOpenkSirc/enter_combo.h)
add_library(ksopenksirc STATIC
    KSOpenkSirc/open_ksirc.cpp
    KSOpenkSirc/open_ksircData.cpp
    KSOpenkSirc/serverFileParser.cpp
    KSOpenkSirc/enter_combo.cpp ${KSOPENKSIRC_MOC})

TQT3_WRAP_CPP(PWS_MOC
    ${CMAKE_CURRENT_SOURCE_DIR}/pws-0.5/pws/pws.h ${CMAKE_CURRENT_SOURCE_DIR}/pws-0.5/pws/pwswidget.h ${CMAKE_CURRENT_SOURCE_DIR}/pws-0.5/pws/pages.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pws-0.5/pws/addwizard.h ${CMAKE_CURRENT_SOURCE_DIR}/pws-0.5/pws/server.h)
add_library(pws STATIC
    pws-0.5/pws/pws.cpp
    pws-0.5/pws/pwswidget.cpp
    pws-0.5/pws/typelayout.cpp
    pws-0.5/pws/tlform.cpp
    pws-0.5/pws/tlwidget.cpp
    pws-0.5/pws/pages.cpp
    pws-0.5/pws/kfileio.cpp
    pws-0.5/pws/addwizard.cpp
    pws-0.5/pws/server.cpp ${PWS_MOC})

# ── puke 动态库(Makefile.am 的 libpuke.la;仅 5 个核心源,
#    其余 p*.cpp 为未完成的实验代码,当年亦不编)──
TQT3_WRAP_CPP(PUKE_MOC
    ${CMAKE_CURRENT_SOURCE_DIR}/puke/controller.h ${CMAKE_CURRENT_SOURCE_DIR}/puke/pwidget.h ${CMAKE_CURRENT_SOURCE_DIR}/puke/pobject.h ${CMAKE_CURRENT_SOURCE_DIR}/puke/playout.h)
add_library(puke SHARED
    puke/controller.cpp
    puke/pwidget.cpp
    puke/pobject.cpp
    puke/playout.cpp
    puke/kdynlib.cpp ${PUKE_MOC})
set_target_properties(puke PROPERTIES VERSION 0.1.0 SOVERSION 0)
# puke 反向引用主程序符号(objFinder 等,运行期经主程序 -rdynamic 导出解析),
# 豁免模块级 --no-undefined(追加在后,覆盖全局)
set_target_properties(puke PROPERTIES LINK_FLAGS "-Wl,--allow-shlib-undefined")
target_link_libraries(puke
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI})
install(TARGETS puke LIBRARY DESTINATION ${KDE1_LIBDIR})

# ── moc:全部 Q_OBJECT 头经 TQT3_WRAP_CPP 生成 .moc;
#    ${*_MOC} 保留在各目标源列表——CMake 不编译 .moc 后缀,此引用仅作
#    custom_command 的生成触发器;实际编译经各 cpp 的 #include "*.moc" 并入 ──
set(ksirc_MOC
    toplevel.h
    servercontroller.h
    ksircprocess.h
    iocontroller.h
    messageReceiver.h
    FilterRuleEditor.h
    FilterRuleEditorData.h
    KSircColour.h
    KSircColourData.h
    ahistlineedit.h
    alistbox.h
    dccDialog.h
    dccDialogData.h
    ioBroadcast.h
    ioDCC.h
    ioDiscard.h
    ioLAG.h
    ioNotify.h
    irclistbox.h
    irclistitem.h
    kpopupmenu.h
    objFinder.h
    open_top.h
    open_topData.h
    ssfeprompt.h
    ssfepromptdata.h
    KMDIMgr.h
    KMDIMgrBase.h
    welcomeWin.h
    welcomeWinData.h
)
TQT3_WRAP_CPP(MOC_FILES ${ksirc_MOC})

set(ksirc_SRCS
    toplevel.cpp
    ksirc.cpp
    irclistbox.cpp
    irclistitem.cpp
    alistbox.cpp
    ahistlineedit.cpp
    iocontroller.cpp
    open_top.cpp open_topData.cpp
    messageReceiver.cpp
    ioBroadcast.cpp
    ksircprocess.cpp
    servercontroller.cpp
    KSircColour.cpp KSircColourData.cpp
    FilterRuleEditor.cpp FilterRuleEditorData.cpp
    ioDiscard.cpp
    ioDCC.cpp dccDialog.cpp dccDialogData.cpp
    linelistitem.cpp
    ioLAG.cpp
    ioNotify.cpp
    ssfeprompt.cpp ssfepromptdata.cpp
    usercontrolmenu.cpp
    kspainter.cpp
    baserules.cpp
    estring.cpp
    chanparser.cpp
    objFinder.cpp
    KMDIMgr.cpp KMDIMgrBase.cpp
    displayMgr.cpp displayMgrMDI.cpp displayMgrSDI.cpp
    welcomeWin.cpp welcomeWinData.cpp
    kpopupmenu.cpp
    ${MOC_FILES})  # getdate.c 已移除:其自带 yacc 测试壳 main 与 ksirc 冲突,且 ksirc 未引用其函数

add_executable(ksirc ${ksirc_SRCS} ${MOC_FILES})
# puke 共享库反向引用主程序符号(objFinder 等),需导出动态符号表
# (当年 Makefile.am 的 -export-dynamic 等价物)
set_target_properties(ksirc PROPERTIES ENABLE_EXPORTS ON)
target_link_options(ksirc PRIVATE -rdynamic)
target_link_libraries(ksirc
    # 静态子库在前(其成员的未解析符号由后面的共享库满足;ld 单遍扫描)
    kplunger ksprogress kstickerlib kscutdialog ksprefs
    kscolourpicker ksopenksirc pws puke
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KSIRC_KFILE}
    ${KDE1_KFM}
    ${CMAKE_DL_LIBS}
)

install(TARGETS ksirc RUNTIME DESTINATION ${KDE1_BINDIR})

# ── ksticker 独立跑马灯程序(当年 bin_PROGRAMS)──
TQT3_WRAP_CPP(KSTICKER_MOC ${CMAKE_CURRENT_SOURCE_DIR}/KSTicker/main.h)
add_executable(ksticker KSTicker/main.cpp KSTicker/kspainter.cpp ${KSTICKER_MOC})
target_link_libraries(ksticker
    kplunger kstickerlib
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI})
install(TARGETS ksticker RUNTIME DESTINATION ${KDE1_BINDIR})
