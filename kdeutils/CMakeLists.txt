cmake_minimum_required (VERSION 3.0)

project (kdeutils)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

# [KDE1 Revival 2026] rpath：把最终安装前缀的库目录烧进 ELF（DT_RUNPATH），
# 用户在任意桌面（如 XFCE）直接运行 /usr/kde1/bin 下程序不再报
# "libtqt-mt.so.3: cannot open shared object file"——无需手设
# LD_LIBRARY_PATH。开发期 staging 树不受影响（LD_LIBRARY_PATH 优先于 rpath）。
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" "${CMAKE_INSTALL_PREFIX}/tqt3/lib")

find_package(KDE1 REQUIRED)
#find_package(uulib REQUIRED)

add_definitions(-DHAVE_CONFIG_H)

include_directories(common)

include(CMakePackageConfigHelpers)
include(TQt3Macros)
include(KDE1Macros)
include(KDE1InstallDirs)

option(ENABLE_SANITIZERS "Enable runtime sanitizing (for development)")
if (ENABLE_SANITIZERS)
    message("Enabling asan and ubsan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-write-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++98 -Wno-write-strings")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
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


#add_subdirectory(kfontmanager) # It is in kdebase
add_subdirectory(kedit)
add_subdirectory(klipper)
add_subdirectory(karm)
add_subdirectory(khexdit)
add_subdirectory(klpq)
add_subdirectory(ktop)
add_subdirectory(kjots)
add_subdirectory(kab)
add_subdirectory(ark)
add_subdirectory(kcalc)
if (CMAKE_SYSTEM_NAME MATCHES "NetBSD")
else()
    if (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    else()
        add_subdirectory(kpm)
        add_subdirectory(kfloppy)
    endif()
endif()
add_subdirectory(kljettool)
add_subdirectory(knotes)
add_subdirectory(kwrite)
# po 子树此前未接入 CMake（zh_CN 翻译从未随构建安装），现启用——
# po/CMakeLists.txt 只挂 zh_CN.UTF-8（其余语言目录仍是 Makefile.am 时代产物）
add_subdirectory(po)

