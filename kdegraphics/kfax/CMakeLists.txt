include_directories(${KDE1_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/libtiffax)
add_definitions(-Dlinux)

# 内嵌 libtiffax（1999 tiff 分支 + G3 传真扩展）：剔除平台特化文件与代码生成器
set(tiffax_SRCS
    libtiffax/tif_aux.c libtiffax/tif_close.c libtiffax/tif_codec.c
    libtiffax/tif_compress.c libtiffax/tif_dir.c libtiffax/tif_dirinfo.c
    libtiffax/tif_dirread.c libtiffax/tif_dirwrite.c libtiffax/tif_dumpmode.c
    libtiffax/tif_error.c libtiffax/tif_fax3.c libtiffax/tif_flush.c
    libtiffax/tif_getimage.c libtiffax/tif_jpeg.c libtiffax/tif_lzw.c
    libtiffax/tif_next.c libtiffax/tif_open.c libtiffax/tif_packbits.c
    libtiffax/tif_predict.c libtiffax/tif_print.c libtiffax/tif_read.c
    libtiffax/tif_strip.c libtiffax/tif_swab.c libtiffax/tif_thunder.c
    libtiffax/tif_tile.c libtiffax/tif_unix.c libtiffax/tif_version.c
    libtiffax/tif_warning.c libtiffax/tif_write.c libtiffax/tif_zip.c
)
# tif_fax3sm.c 由 mkg3states 生成（上游 BUILT_SOURCES 等价物）
add_executable(mkg3states libtiffax/mkg3states.c)
target_include_directories(mkg3states PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libtiffax)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tif_fax3sm.c
    COMMAND mkg3states -c const ${CMAKE_CURRENT_BINARY_DIR}/tif_fax3sm.c
    DEPENDS mkg3states VERBATIM)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/tif_fax3sm.c PROPERTIES GENERATED TRUE)
list(APPEND tiffax_SRCS ${CMAKE_CURRENT_BINARY_DIR}/tif_fax3sm.c)
add_library(tiffax STATIC ${tiffax_SRCS})
# u_char 等 BSD 类型由 glibc _DEFAULT_SOURCE 提供（勿限 _POSIX_SOURCE）

set(kfax_SRCS
    kfax.cpp viewfax.cpp faxexpand.cpp faxinit.cpp faxinput.cpp
    fax2tiff.c fax2ps.c print.cpp options.cpp urldlg.cpp
)
set(kfax_MOC kfax.h print.h options.h urldlg.h)
TQT3_WRAP_CPP(MOC_FILES ${kfax_MOC})

add_executable(kfax1 ${kfax_SRCS} ${MOC_FILES})
target_link_libraries(kfax1 tiffax
    ${TQT3_LIBRARIES} ${X11_LIBRARIES} m jpeg tiff z
    ${KDE1_KDECORE} ${KDE1_KDEUI} ${KDE1_KFILE} ${KDE1_KFM})

install(TARGETS kfax1 RUNTIME DESTINATION ${KDE1_BINDIR})
install_icon(kfax.xpm "${KDE1_ICONDIR}")
install(FILES mini-kfax.xpm DESTINATION ${KDE1_ICONDIR}/mini RENAME kfax.xpm)
install(FILES KFax.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
# fax-g3.kdelnk 与 kdelibs 自带的 mime 声明重复（dpkg 覆盖冲突），不再重复安装
