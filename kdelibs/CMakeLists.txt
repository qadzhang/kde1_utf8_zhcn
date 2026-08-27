cmake_minimum_required (VERSION 3.0)

project (kdelibs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (NOT CMAKE_BUILD_TYPE)
    #        set(CMAKE_BUILD_TYPE Release)
endif()

# additional cmake files
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(TIFF REQUIRED)
find_package(X11 REQUIRED)
find_package(Qt1 REQUIRED)
find_package(Gettext REQUIRED)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(Qt1Macros)
include(KDE1Macros)

set(KDE1_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(KDE1_APPSDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/applnk")
set(KDE1_BINDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set(KDE1_CGIDIR "${KDE1_PREFIX}/cgi-bin")
set(KDE1_CONFDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/config")
set(KDE1_DATADIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/apps")
set(KDE1_HTMLDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/doc/kde1/HTML")
set(KDE1_ICONDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons")
set(KDE1_INCLUDES "${KDE1_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/kde1")
set(KDE1_LIBRARIES "${KDE1_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(KDE1_LIBDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(KDE1_LOCALE "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/locale/kde1/")
set(KDE1_MIMEDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/mimelnk")
set(KDE1_MINIDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/icons/mini")
set(KDE1_PARTSDIR "${KDE1_PREFIX}/parts")
set(KDE1_SOUNDDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/sounds")
set(KDE1_TOOLBARDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/toolbar")
set(KDE1_WALLPAPERDIR "${KDE1_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/wallpapers")

set(KDELIBS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

configure_package_config_file(cmake/KDE1InstallDirs.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/KDE1InstallDirs.cmake
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}/KDE1/cmake
    )

configure_file(common/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DHAVE_CONFIG_H )

option(ENABLE_SANITIZERS "Enable runtime sanitizing (for development)")
if (ENABLE_SANITIZERS)
    message("Enabling asan and ubsan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
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

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wno-write-strings -Wno-deprecated-declarations -Wno-unused")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-write-strings -Wno-deprecated-declarations -Wno-unused")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive -std=c++98")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wno-format-security -Wno-dangling-else")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-format-security -Wno-dangling-else")
endif()

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/KDE1InstallDirs.cmake
    cmake/FindKDE1.cmake
    cmake/KDE1Macros.cmake
    cmake/KDE1Macros.cmake
    cmake/Qt1Macros.cmake
    DESTINATION ${CMAKE_ROOT}/Modules)

add_subdirectory(kdecore)
add_subdirectory(kdeui)
add_subdirectory(kdetest)
add_subdirectory(kfile)
add_subdirectory(kfmlib)
add_subdirectory(jscript)
add_subdirectory(kimgio)
add_subdirectory(khtmlw)
add_subdirectory(mediatool)
add_subdirectory(kspell)
add_subdirectory(kab)
add_subdirectory(toolbar)
add_subdirectory(doc)
add_subdirectory(po)
