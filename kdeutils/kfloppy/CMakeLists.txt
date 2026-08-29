# FIXME: at least use `uname -r`
#set(KERNEL_INCLUDE_DIR /usr/src/kernels/4.8.3-300.fc25.x86_64)

include_directories(
    ${KDE1_INCLUDE_DIR}
#    ${KERNEL_INCLUDE_DIR}/include
#    ${KERNEL_INCLUDE_DIR}/arch/x86/include/
)

set(KFLOPPY_SRCS
    main.cpp about.cpp floppy.cpp mount.c
)

set(KFLOPPY_MOC
    about.h floppy.h
)

TQT3_WRAP_CPP(KFLOPPY_MOC_FILES ${KFLOPPY_MOC})

add_executable(kfloppy1 ${KFLOPPY_SRCS} ${KFLOPPY_MOC_FILES})
target_link_libraries(kfloppy1
    ${TQT3_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kfloppy1 RUNTIME DESTINATION ${KDE1_BINDIR})

install(FILES KFloppy.kdelnk DESTINATION ${KDE1_APPSDIR}/Utilities)



install_icon(kfloppy.xpm "${KDE1_ICONDIR}")
install_icon(kfloppylogo.xpm "${KDE1_DATADIR}/kfloppy")

install(FILES
    doc/kfloppy-1.html
    doc/kfloppy-2.html
    doc/kfloppy-3.html
    doc/kfloppy-4.html
    doc/kfloppy-5.html
    doc/kfloppy.html
    doc/kfloppy.nif
    doc/kfloppy.sgml
    doc/logotp3.gif
    DESTINATION ${KDE1_HTMLDIR}/en/kfloppy)

######################### Next target ########################

# kfloppy 运行期依赖的三个辅助程序(floppy.cpp 探测 kde_bindir 下可执行):
# kfdformat(格式化内核)/kmkdosfs(DOS 文件系统)——kmke2fs 无源码(外部 e2fsprogs)
set(KMKDOSFS_SRCS kmkdosfs.c)
add_executable(kmkdosfs ${KMKDOSFS_SRCS})
install(TARGETS kmkdosfs RUNTIME DESTINATION ${KDE1_BINDIR})

set(KFDFORMAT_SRCS kfdformat.c)
add_executable(kfdformat ${KFDFORMAT_SRCS})
install(TARGETS kfdformat RUNTIME DESTINATION ${KDE1_BINDIR})

######################## Next target ########################



