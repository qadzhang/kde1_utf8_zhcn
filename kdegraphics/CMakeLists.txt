cmake_minimum_required (VERSION 3.0)

project (kdegraphics)

# kdelibs 源树的 cmake 模块路径（FindKPathsea 等自 kdelibs 携带）
set(KDELIBS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../kdelibs")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

# [KDE1 Revival 2026] rpath：把最终安装前缀的库目录烧进 ELF（DT_RUNPATH），
# 用户在任意桌面（如 XFCE）直接运行 /usr/kde1/bin 下程序不再报
# "libtqt-mt.so.3: cannot open shared object file"——无需手设
# LD_LIBRARY_PATH。开发期 staging 树不受影响（LD_LIBRARY_PATH 优先于 rpath）。
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" "${CMAKE_INSTALL_PREFIX}/tqt3/lib")

list(APPEND CMAKE_MODULE_PATH "${KDELIBS_SOURCE_DIR}/cmake")
find_package(KDE1 REQUIRED)
# kimgio 共享库在 kdelibs 产物树（与 kdenetwork 同款查找方式）
find_library(KDE1_KIMGIO kimgio ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})

add_definitions(-DHAVE_CONFIG_H)

# 手写移植版 config.h（照 kdeutils/kdenetwork 的 common/config.h 惯例）
include_directories(common)

include(CMakePackageConfigHelpers)
include(TQt3Macros)
include(KDE1Macros)
include(KDE1InstallDirs)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-write-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=gnu++98 -Wno-write-strings")
endif()

# 1999 年图形应用全家：截图/看图/画图/图标编辑/分形/DVI/传真/PS 查看
add_subdirectory(ksnapshot)
add_subdirectory(kview)
add_subdirectory(kpaint)
add_subdirectory(kiconedit)
add_subdirectory(kfract)
add_subdirectory(kdvi)
add_subdirectory(kfax)
add_subdirectory(kghostview)
# po 子树只挂 zh_CN.UTF-8（与其它模块一致）
add_subdirectory(po)
