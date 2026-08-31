cmake_minimum_required (VERSION 3.0)

project (kdenetwork)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

# [KDE1 Revival 2026] rpath：把最终安装前缀的库目录烧进 ELF（DT_RUNPATH），
# 用户在任意桌面（如 XFCE）直接运行 /usr/kde1/bin 下程序不再报
# "libtqt-mt.so.3: cannot open shared object file"——无需手设
# LD_LIBRARY_PATH。开发期 staging 树不受影响（LD_LIBRARY_PATH 优先于 rpath）。
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" "${CMAKE_INSTALL_PREFIX}/tqt3/lib")

find_package(KDE1 REQUIRED)

# ── 库路径修正(2026 TQt3 迁移):系统 /usr/kde1/lib 残留 Qt1 时代同名旧库,
#    find_library 首查会命中旧库(符号不兼容)且结果进缓存;
#    先 unset 缓存再以 staging 路径优先重找 ──
foreach(_k KDE1_KDECORE KDE1_KDEUI KDE1_KFILE KDE1_KFM KDE1_KHTMLW KDE1_KSPELL KDE1_KIMGIO)
    unset(${_k} CACHE)
endforeach()
find_library(KDE1_KDECORE kdecore ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KDE1_KDEUI  kdeui  ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KDE1_KFILE  kfile  ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KDE1_KFM    kfm    ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KDE1_KHTMLW khtmlw ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KDE1_KSPELL kspell ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
find_library(KDE1_KIMGIO kimgio ${CMAKE_SOURCE_DIR}/../staging/usr/kde1/lib ${KDE1_LIBRARIES})
#find_package(uulib REQUIRED)

add_definitions(-DHAVE_CONFIG_H)

include_directories(common)

include(CMakePackageConfigHelpers)
include(TQt3Macros)
include(KDE1Macros)
include(KDE1InstallDirs)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-write-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++98")
    # 注:原先此处收紧 -Wl,--no-undefined;ksirc 的 libpuke 需运行期经主程序
    # (-rdynamic)解析 objFinder 等符号,so 自身 undefined 由 ld 默认放行
endif()

if (CMAKE_SYSTEM_NAME MATCHES "NetBSD")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -I/usr/X11R7/include -I/usr/pkg/include -I/usr/local/include -I/usr/kde1/include -L/usr/X11R7/lib -L/usr/pkg/lib -L/usr/local/lib -L/usr/kde1/lib -Wl,-R/usr/X11R7/lib -Wl,-R/usr/pkg/lib -Wl,-R/usr/local/lib -Wl,-R/usr/kde1/lib")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -I/usr/X11R7/include -I/usr/pkg/include -I/usr/local/include -I/usr/kde1/include -L/usr/X11R7/lib -L/usr/pkg/lib -L/usr/local/lib -L/usr/kde1/lib -Wl,-R/usr/X11R7/lib -Wl,-R/usr/pkg/lib -Wl,-R/usr/local/lib -Wl,-R/usr/kde1/lib")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L/usr/X11R7/lib -L/usr/pkg/lib -L/usr/local/lib -L/usr/kde1/lib -R/usr/X11R7/lib -R/usr/pkg/lib -R/usr/local/lib -R/usr/kde1/lib")
elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -I/usr/local/include -I/usr/kde1/include")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -I/usr/local/include -I/usr/kde1/include")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L/usr/local/lib -L/usr/kde1/lib -R/usr/local/lib -R/usr/kde1/lib")
elseif (CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -I/usr/local/include -I/usr/kde1/include -I/usr/X11R6/include")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -I/usr/local/include -I/usr/kde1/include -I/usr/X11R6/include")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L/usr/local/lib -L/usr/kde1/lib -L/usr/X11R6/lib -R/usr/local/lib -R/usr/kde1/lib -R/usr/X11R6/lib")
endif()

add_subdirectory(karchie)
add_subdirectory(kbiff)
add_subdirectory(kfinger)
add_subdirectory(kmail)
add_subdirectory(knu)
add_subdirectory(korn)
add_subdirectory(kppp)
add_subdirectory(krn)
add_subdirectory(ksirc)
#add_subdirectory(ktalkd)  # talk 协议守护进程:现代系统无 talk/inetd 生态,非 GUI 应用,不纳入交付
add_subdirectory(mimelib)
add_subdirectory(po)
