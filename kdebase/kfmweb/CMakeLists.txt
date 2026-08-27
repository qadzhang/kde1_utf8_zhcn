# [KDE1 Revival 2026] kfm 新内核视图（WebKit2GTK，见 kde1-webview.c 头注释）
find_package(PkgConfig REQUIRED)
pkg_check_modules(WEBVIEW REQUIRED webkit2gtk-4.1 gtk+-3.0)

add_executable(kde1-webview kde1-webview.c)
target_include_directories(kde1-webview PRIVATE ${WEBVIEW_INCLUDE_DIRS})
target_link_libraries(kde1-webview ${WEBVIEW_LIBRARIES})

install(TARGETS kde1-webview RUNTIME DESTINATION ${KDE1_BINDIR})
