# 查找 TeX kpathsea 路径搜索库（kdvi 字体查找依赖）
find_path(KPATHSEA_INCLUDE_DIR kpathsea/kpathsea.h)
find_library(KPATHSEA_LIBRARY NAMES kpathsea)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KPathsea DEFAULT_MSG KPATHSEA_LIBRARY KPATHSEA_INCLUDE_DIR)
mark_as_advanced(KPATHSEA_INCLUDE_DIR KPATHSEA_LIBRARY)
