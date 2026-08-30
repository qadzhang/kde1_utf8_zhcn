include_directories(${KDE1_INCLUDE_DIR})
add_definitions(-Dlinux)

# 源文件是 .C 后缀（kfract 上游习惯）
set(kfract_SRCS
    drawview.C fract.C julia.C kfract.C kfractdlgs.C lambda.C
    mandel.C main.C
)
set(kfract_MOC kfract.h kfractdlgs.h)
TQT3_WRAP_CPP(MOC_FILES ${kfract_MOC})

add_executable(kfract1 ${kfract_SRCS} ${MOC_FILES})
target_link_libraries(kfract1
    ${TQT3_LIBRARIES} ${X11_LIBRARIES} m
    ${KDE1_KDECORE} ${KDE1_KDEUI})

install(TARGETS kfract1 RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kfract.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
# [KDE1 Revival 2026] 三尺寸图标变体与 kdelnk 的 Icon= 引用配套
install_icon(kfract.xpm "")
