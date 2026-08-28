#=============================================================================
# FindTQt3.cmake — 探测 TQt3 底座并接通 KDE1→TQt3 迁移脚手架
#
# What : 定位 TQt3 的头文件/库/tqmoc，输出使用接口；并把脚手架主头
#        q1compat.h 以 -include 注入编译命令（Qt1 名字→TQt3 现实的映射）。
# Why  : KDE 1.1.2 源码以 Qt1 名字编写，TQt3 全部 TQ 前缀化；注入映射让
#        尚未显式 TQ 化的模块直接编译（strangler fig 脚手架——模块显式化
#        完成后置 TQT3_NO_Q1COMPAT 关闭注入即摘除；全模块摘除后随 port/ 拆除）。
# Who  : kdelibs 及其后的全部 KDE1 模块在 CMakeLists 中 find_package(TQt3)。
# When : 各模块 cmake 配置期。
# Where: kdelibs/cmake/FindTQt3.cmake；探测目标来自 build.sh 暂存的
#        staging/<prefix>/tqt3（经 TQTDIR 环境变量或 CMAKE_PREFIX_PATH）。
# How  : 伪代码：
#   1. find_path ntqglobal.h（候选：$TQTDIR/include、<prefix>/tqt3/include）
#   2. find_library tqt-mt（同前缀 lib）
#   3. find_program tqmoc（同前缀 bin）
#   4. 输出 TQT3_INCLUDE_DIRS = tqt3/include + 仓库根/port/include（Qt1 缩写
#      转发头）+ 仓库根/port（q1compat_map.h）
#   5. 输出 TQT3_DEFINITIONS = -DQT_THREAD_SUPPORT -include q1compat.h
#      （TQT3_NO_Q1COMPAT 置 ON 时去掉 -include——模块显式化后的摘除开关）
#=============================================================================

set(TQT3_FOUND 0)

# 探测根目录候选（What/Why）：优先 CMAKE_PREFIX_PATH 下的 tqt3（build.sh 传入的
# staging 暂存区——持久视图）；ENV{TQTDIR} 是构建树路径，clean.sh 清理后会悬垂，
# 仅作回退；最后是 deb 安装后的系统真实路径
set(_TQT3_ROOT_HINTS "${CMAKE_PREFIX_PATH}/tqt3" "$ENV{TQTDIR}" "/usr/kde1/tqt3")

find_path(TQT3_INCLUDE_DIR ntqglobal.h
    HINTS ${_TQT3_ROOT_HINTS}
    PATH_SUFFIXES include
)

find_library(TQT3_LIBRARY NAMES tqt-mt libtqt-mt
    HINTS ${_TQT3_ROOT_HINTS}
    PATH_SUFFIXES lib
)

find_program(TQT3_MOC_EXECUTABLE NAMES tqmoc
    HINTS ${_TQT3_ROOT_HINTS}
    PATH_SUFFIXES bin
)

if (TQT3_INCLUDE_DIR AND TQT3_LIBRARY AND TQT3_MOC_EXECUTABLE)
    set(TQT3_FOUND 1)
    # 仓库根（What/Why）：优先显式 KDE1_REPO_ROOT（build.sh 传入——本文件被
    # 安装到 staging 的 cmake Modules 后，CMAKE_CURRENT_LIST_DIR 推不出仓库
    # 根）；源内使用时（kdelibs/cmake/ 下）回退路径推算仍有效
    if (NOT KDE1_REPO_ROOT)
        set(_KDE1_REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")
        get_filename_component(_KDE1_REPO_ROOT "${_KDE1_REPO_ROOT}" ABSOLUTE)
    else ()
        set(_KDE1_REPO_ROOT "${KDE1_REPO_ROOT}")
    endif ()

    set(TQT3_INCLUDE_DIRS
        "${TQT3_INCLUDE_DIR}"
        "${_KDE1_REPO_ROOT}/port/include"
        "${_KDE1_REPO_ROOT}/port"
    )
    set(TQT3_LIBRARIES ${TQT3_LIBRARY})

    set(TQT3_DEFINITIONS "-DQT_THREAD_SUPPORT")
    # 脚手架注入（摘除开关：模块显式 TQ 化后 set(TQT3_NO_Q1COMPAT ON)）。
    # What/Why：-include 必须只对 C++ 编译单元生效——模块里存在纯 C 文件
    #（如 kdecore/convert_key.c），C 编译器被注入 C++ 头会连 <string> 都
    # 找不到而雪崩；故不走 add_definitions（C/C++ 通吃），挂 CMAKE_CXX_FLAGS
    if (NOT TQT3_NO_Q1COMPAT)
        set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -include${_KDE1_REPO_ROOT}/port/q1compat.h")
    endif()

    # TQt3 构建树内 mkspecs 里的配置头（qconfig 等）已在 include/ 下，无需额外路径

    # 全局注入（What/Why）：头路径直接作用于 find_package 的调用目录并向下
    # 传播，子目录 CMakeLists 无需逐个 include_directories——与旧
    # ${TQT3_LIBRARIES} 隐式携带 include 的行为等价替换（C 文件多一路无害）
    include_directories(${TQT3_INCLUDE_DIRS})
    add_definitions(${TQT3_DEFINITIONS})

    message(STATUS "找到 TQt3：${TQT3_LIBRARY}")
    message(STATUS "  tqmoc：${TQT3_MOC_EXECUTABLE}")
    if (NOT TQT3_NO_Q1COMPAT)
        message(STATUS "  已注入 q1compat 脚手架（-include）")
    endif ()
endif ()

mark_as_advanced(TQT3_INCLUDE_DIR TQT3_LIBRARY TQT3_MOC_EXECUTABLE)
