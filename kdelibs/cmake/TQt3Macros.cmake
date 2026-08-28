#=============================================================================
# TQt3Macros.cmake — tqmoc 包装宏（含 Q_OBJECT→TQ_OBJECT 预替换）
#
# What : 提供 TQT3_WRAP_HEADER / TQT3_WRAP_CPP 宏，等价于旧 QT1_WRAP_*：
#        对声明了信号槽的头/源文件跑 tqmoc 生成 .moc。
# Why  : tqmoc 的自带词法器只认 TQ_OBJECT 等 TQt3 宏名、不展开 #define，
#        而 KDE 1.1.2 源码全用 Q_OBJECT；故先用 sed 生成替换副本再送 moc。
#        （编译器一侧由 q1compat.h 的 #define Q_OBJECT TQ_OBJECT 覆盖——
#         双轨各管一边：moc 吃 sed 副本，编译器吃宏映射。）
# Who  : 各 KDE1 模块的 CMakeLists 调用；由 FindTQt3.cmake 一并 include。
# When : 构建期（add_custom_command），输入变更自动重跑。
# Where: kdelibs/cmake/TQt3Macros.cmake。
# How  : 伪代码：
#   TQT3_WRAP_CPP(outfiles src1.h src2.cpp ...)
#     对每个输入 F：
#       sed（六宏替换：Q_OBJECT/Q_PROPERTY/Q_ENUMS/Q_SETS/Q_CLASSINFO/
#            Q_OVERRIDE → TQ_*）F → <build>/F.q1moc（副本，路径展平加序号防撞）
#       tqmoc <build>/F.q1moc -o <build>/<name>.moc
#     outfiles 收集全部 .moc（KDE1 风格：源文件末尾 #include "xxx.moc"）
#=============================================================================

macro (TQT3_WRAP_CPP outfiles)
    set(_i 0)
    foreach (_it ${ARGN})
        # 绝对路径直接用（port/ 脚手架源等仓库级输入）；相对路径以当前源目录为基
        if (IS_ABSOLUTE "${_it}")
            set(_infile ${_it})
        else ()
            set(_infile ${CMAKE_CURRENT_SOURCE_DIR}/${_it})
        endif ()
        get_filename_component(_outfile ${_it} NAME_WE)
        # 展平副本名：<名>.<序号>.q1moc——同名录文件在不同子目录也不相撞
        set(_copy ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.${_i}.q1moc)
        set(_mocfile ${CMAKE_CURRENT_BINARY_DIR}/${_outfile}.moc)
        # What/Why：-f 强制 moc 输出顶部生成 #include "原名"（sed 副本输入会使
        # 默认 include 指向副本名——KDE1 的 CMake 把 ${MOC_FILES} 当独立编译
        # 单元，moc 输出必须自带类定义）；-p 补源目录前缀使 include 可解析；
        # -f 只用 basename（防与 -p 前缀双拼）
        get_filename_component(_fname ${_it} NAME)
        # -p 前缀：绝对路径输入（port/ 脚手架源等）用其自身目录，否则当前源目录
        if (IS_ABSOLUTE "${_it}")
            get_filename_component(_pdir "${_it}" PATH)
        else ()
            set(_pdir "${CMAKE_CURRENT_SOURCE_DIR}")
        endif ()
        add_custom_command(OUTPUT ${_mocfile}
            COMMAND sed -e s/\\bQ_OBJECT\\b/TQ_OBJECT/g -e s/\\bQ_PROPERTY\\b/TQ_PROPERTY/g -e s/\\bQ_ENUMS\\b/TQ_ENUMS/g -e s/\\bQ_SETS\\b/TQ_SETS/g -e s/\\bQ_CLASSINFO\\b/TQ_CLASSINFO/g -e s/\\bQ_OVERRIDE\\b/TQ_OVERRIDE/g ${_infile} > ${_copy}
            COMMAND ${TQT3_MOC_EXECUTABLE} ${_copy} -f${_fname} -p${_pdir}/ -o ${_mocfile}
            MAIN_DEPENDENCY ${_infile}
            COMMENT "tqmoc（经 Q_OBJECT→TQ_OBJECT 预替换）：${_it}"
            VERBATIM)
        list(APPEND ${outfiles} ${_mocfile})
        math(EXPR _i "${_i}+1")
    endforeach ()
endmacro ()
